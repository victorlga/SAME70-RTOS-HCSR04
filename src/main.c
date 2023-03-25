#include <asf.h>
#include "conf_board.h"

#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "gfx_mono_generic.h"
#include "sysfont.h"

#define TRIGGER_PIO	   PIOA
#define TRIGGER_PIO_ID ID_PIOA
#define TRIGGER_IDX    2
#define TRIGGER_IDX_MASK (1 << TRIGGER_IDX)

#define ECHO_PIO       PIOA
#define ECHO_PIO_ID    ID_PIOA
#define ECHO_IDX       24
#define ECHO_IDX_MASK  (1 << ECHO_IDX)

/** RTOS  */
#define TASK_OLED_STACK_SIZE                (1024*6/sizeof(portSTACK_TYPE))
#define TASK_OLED_STACK_PRIORITY            (tskIDLE_PRIORITY)

SemaphoreHandle_t xSemaphoreEcho;

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,  signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);

/** prototypes */
void signal_callback(void);
static void ECHO_init(void);
static void task_oled(void *pvParameters);
static void task_trig(void *pvParameters);


/************************************************************************/
/* RTOS application funcs                                               */
/************************************************************************/

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName) {
	printf("stack overflow %x %s\r\n", pxTask, (portCHAR *)pcTaskName);
	for (;;) {	}
}

extern void vApplicationIdleHook(void) { }

extern void vApplicationTickHook(void) { }

extern void vApplicationMallocFailedHook(void) {
	configASSERT( ( volatile void * ) NULL );
}

/************************************************************************/
/* handlers / callbacks                                                 */
/************************************************************************/

void signal_callback(void) {
	if (pio_get(ECHO_PIO, PIO_INPUT, ECHO_IDX_MASK)) {
		rtt_init(RTT, 1);
	} else {
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR(xSemaphoreEcho, &xHigherPriorityTaskWoken);
	}
}

/************************************************************************/
/* TASKS                                                                */
/************************************************************************/

static void task_oled(void *pvParameters) {
	typedef struct {
		int chart_x;
		int distance;
	} measure;
	
 	gfx_mono_ssd1306_init();
	 
	int counter = 20;
	for (;;)  {
		// Para usar delay usar fila
		if (xSemaphoreTake(xSemaphoreEcho, 0)) {
			int ul_previous_time = rtt_read_timer_value(RTT);
			printf("Retorno do sensor: %u\n",ul_previous_time);
			
			// Transforma em us
			int delay = ul_previous_time * 1000000/ 32768;
			// Transforma em cm
			int dist = delay * 340 / (2 * 10000);
			
			
			if (counter >= 128) {
				counter = 20;
				gfx_mono_ssd1306_init();
			}
			
			gfx_mono_generic_draw_filled_circle(counter, 32 - dist / 16, 1, GFX_PIXEL_SET, GFX_WHOLE);
			
			char str;
			gfx_mono_draw_string("   ", 0, 0, &sysfont);
			sprintf(str, "%d", dist);
			gfx_mono_draw_string(str, 0, 0, &sysfont);
			gfx_mono_draw_string("cm", 0, 10, &sysfont);
			
			counter++;
			
			printf("Distancia: %d cm\n", dist);
		}
	}
}

static void task_trig(void *pvParameters) {

	for (;;)  {
		pio_set(TRIGGER_PIO, TRIGGER_IDX_MASK);
		delay_us(10);
		pio_clear(TRIGGER_PIO, TRIGGER_IDX_MASK);
		printf("Trigger\n");
		vTaskDelay(250 / portTICK_PERIOD_MS);
	}
}

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

static void configure_console(void) {
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.charlength = CONF_UART_CHAR_LENGTH,
		.paritytype = CONF_UART_PARITY,
		.stopbits = CONF_UART_STOP_BITS,
	};

	/* Configure console UART. */
	stdio_serial_init(CONF_UART, &uart_serial_options);

	/* Specify that stdout should not be buffered. */
	setbuf(stdout, NULL);
}

void ECHO_init(void) {
	
	pmc_enable_periph_clk(TRIGGER_PIO_ID);
	pio_configure(TRIGGER_PIO, PIO_OUTPUT_0, TRIGGER_IDX_MASK, PIO_DEFAULT);

	pmc_enable_periph_clk(ECHO_PIO_ID);

	pio_configure(ECHO_PIO, PIO_INPUT, ECHO_IDX_MASK, PIO_DEFAULT);

	pio_handler_set(ECHO_PIO,
					ECHO_PIO_ID,
					ECHO_IDX_MASK,
					PIO_IT_EDGE,
					&signal_callback);

	pio_enable_interrupt(ECHO_PIO, ECHO_IDX_MASK);

	NVIC_EnableIRQ(ECHO_PIO_ID);
	NVIC_SetPriority(ECHO_PIO_ID, 4);
}

/************************************************************************/
/* main                                                                 */
/************************************************************************/


int main(void) {
	/* Initialize the SAM system */
	sysclk_init();
	board_init();
	ECHO_init();

	/* Initialize the console uart */
	configure_console();
	
	xSemaphoreEcho = xSemaphoreCreateBinary();
	if (xSemaphoreEcho == NULL)
		 printf("falha em criar o semaforo \n");

	/* Create task to control oled */
	if (xTaskCreate(task_oled, "oled", TASK_OLED_STACK_SIZE, NULL, TASK_OLED_STACK_PRIORITY, NULL) != pdPASS) 
		 printf("Failed to create oled task\r\n");
	
	if (xTaskCreate(task_trig, "oled", TASK_OLED_STACK_SIZE, NULL, TASK_OLED_STACK_PRIORITY, NULL) != pdPASS)
		 printf("Failed to create trig task\r\n");
	
	/* Start the scheduler. */
	vTaskStartScheduler();

  /* RTOS não deve chegar aqui !! */
	while(1);

	/* Will only get here if there was insufficient memory to create the idle task. */
	return 0;
}
