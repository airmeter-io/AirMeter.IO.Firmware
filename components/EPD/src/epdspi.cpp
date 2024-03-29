/* SPI Master IO class */
#include <EpdSpi.h>
#include <string.h>
#include "freertos/task.h"
#include "esp_log.h"
#ifdef CONFIG_IDF_TARGET_ESP32
    #define EPD_HOST    HSPI_HOST
    #define DMA_CHAN    2

#elif defined CONFIG_IDF_TARGET_ESP32S2
    #define EPD_HOST    SPI2_HOST
    #define DMA_CHAN    EPD_HOST

#elif defined CONFIG_IDF_TARGET_ESP32C3
    // chip only support spi dma channel auto-alloc
    #define EPD_HOST    SPI2_HOST
    #define DMA_CHAN    SPI_DMA_CH_AUTO
#endif

void EpdSpi::init(uint8_t frequency=4,bool debug=false){
    debug_enabled = debug;

    //Initialize GPIOs direction & initial states
    gpio_set_direction((gpio_num_t)CONFIG_EINK_SPI_CS, GPIO_MODE_OUTPUT);
    gpio_set_direction((gpio_num_t)CONFIG_EINK_DC, GPIO_MODE_OUTPUT);
    gpio_set_direction((gpio_num_t)CONFIG_EINK_RST, GPIO_MODE_OUTPUT);
    gpio_set_direction((gpio_num_t)CONFIG_EINK_BUSY, GPIO_MODE_INPUT);
    gpio_set_pull_mode((gpio_num_t)CONFIG_EINK_BUSY, GPIO_PULLUP_ONLY);
   
    gpio_set_level((gpio_num_t)CONFIG_EINK_SPI_CS, 1);
    gpio_set_level((gpio_num_t)CONFIG_EINK_DC, 1);
    gpio_set_level((gpio_num_t)CONFIG_EINK_RST, 1);
     gpio_hold_en((gpio_num_t)CONFIG_EINK_RST);
      gpio_hold_en((gpio_num_t)CONFIG_EINK_DC);
    esp_err_t ret;
    // MISO not used, only Master to Slave
    spi_bus_config_t buscfg={
        .mosi_io_num=CONFIG_EINK_SPI_MOSI,
        .miso_io_num = -1,
        .sclk_io_num=CONFIG_EINK_SPI_CLK,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1,
        .max_transfer_sz=4094
    };
    // max_transfer_sz   4Kb is the defaut SPI transfer size if 0
    // debug: 50000  0.5 Mhz so we can sniff the SPI commands with a Slave
    uint16_t multiplier = 1000;
    if (debug_enabled) {
        frequency = 50;
        multiplier = 1;
    }

    printf("CS: %d\n", (int)CONFIG_EINK_SPI_CS);
    //Config Frequency and SS GPIO
    spi_device_interface_config_t devcfg={
        .mode=0,  //SPI mode 0
        .clock_speed_hz=frequency*multiplier*1000,  // DEBUG: 50000 - No debug usually 4 Mhz
        .input_delay_ns=0,
        .spics_io_num=CONFIG_EINK_SPI_CS,
        .flags = (SPI_DEVICE_HALFDUPLEX /*| SPI_DEVICE_3WIRE*/),
        .queue_size=5
    };
    // DISABLED Callbacks pre_cb/post_cb. SPI does not seem to behave the same
    // CS / DC GPIO states the usual way

    //Initialize the SPI bus
    ret=spi_bus_initialize(EPD_HOST, &buscfg, DMA_CHAN);
    ESP_ERROR_CHECK(ret);

    //Attach the EPD to the SPI bus
    ret=spi_bus_add_device(EPD_HOST, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);
    
    //if (debug_enabled) {
      printf("EpdSpi::init() Debug enabled. SPI master at frequency:%d  MOSI:%d CLK:%d CS:%d DC:%d RST:%d BUSY:%d DMA_CH: %d\n",
      frequency*multiplier*1000, CONFIG_EINK_SPI_MOSI, CONFIG_EINK_SPI_CLK, CONFIG_EINK_SPI_CS,
      CONFIG_EINK_DC,CONFIG_EINK_RST,CONFIG_EINK_BUSY, DMA_CHAN);
      //  } else {
     //      printf("EpdSPI started at frequency: %d000\n", frequency*multiplier);
      //  }
    }

/* Send a command to the LCD. Uses spi_device_polling_transmit, which waits
 * until the transfer is complete.
 *
 * Since command transactions are usually small, they are handled in polling
 * mode for higher speed. The overhead of interrupt transactions is more than
 * just waiting for the transaction to complete.
 */
void EpdSpi::cmd(const uint8_t cmd)
{
    if (debug_enabled) {
        printf("C %x\n",cmd);
    } 

    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=8;                     //Command is 8 bits
    t.tx_buffer=&cmd;               //The data is the cmd itself 
    // No need to toogle CS when spics_io_num is defined in SPI config struct
    //gpio_set_level((gpio_num_t)CONFIG_EINK_SPI_CS, 0);
    gpio_hold_dis((gpio_num_t)CONFIG_EINK_DC);
    gpio_set_level((gpio_num_t)CONFIG_EINK_DC, 0);
    gpio_hold_en((gpio_num_t)CONFIG_EINK_DC);
    ret=spi_device_transmit(spi, &t);

    assert(ret==ESP_OK);
    gpio_hold_dis((gpio_num_t)CONFIG_EINK_DC);
    gpio_set_level((gpio_num_t)CONFIG_EINK_DC, 1);
    gpio_hold_en((gpio_num_t)CONFIG_EINK_DC);
    
}

void EpdSpi::data(uint8_t data)
{
    /* if (debug_enabled) {
      printf("D %x\n",data);
    } */
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=8;                     //Command is 8 bits
    t.tx_buffer=&data;              //The data is the cmd itself
    ret=spi_device_transmit(spi, &t);
    
    assert(ret==ESP_OK);
}

void EpdSpi::testRead() {

    uint8_t cmd = 0x2d;
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=8;                     //Command is 8 bits
    t.tx_buffer=&cmd;               //The data is the cmd itself
    t.user=(void*)0;                //D/C needs to be set to 0
    ret=spi_device_transmit(spi, &t);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.

while(true) {
    
      if (gpio_get_level((gpio_num_t)CONFIG_EINK_BUSY) == 0) break;
    
  }
    memset(&t, 0, sizeof(t));
    uint8_t buf[100];
    memset(buf, 0, sizeof(buf));
    t.rxlength=8*11;
    t.rx_buffer = buf;
   //t.flags = SPI_TRANS_USE_RXDATA;
    t.user = (void*)1;

    ret = spi_device_transmit(spi, &t);
    assert( ret == ESP_OK );

}


void EpdSpi::dataBuffer(uint8_t data)
{
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=8;                     //Command is 8 bits
    t.tx_buffer=&data;
    spi_device_transmit(spi, &t);
}

/* Send data to the SPI. Uses spi_device_polling_transmit, which waits until the
 * transfer is complete.
 *
 * Since data transactions are usually small, they are handled in polling
 * mode for higher speed. The overhead of interrupt transactions is more than
 * just waiting for the transaction to complete.
 */
void EpdSpi::data(const uint8_t *data, int len)
{
  if (len==0) return; 

    esp_err_t ret;
    spi_transaction_t t;
                
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=len*8;                 //Len is in bytes, transaction length is in bits.
    t.tx_buffer=data;               //Data
    ret=spi_device_transmit(spi, &t);  //Transmit!
   
    assert(ret==ESP_OK);            //Should have had no issues.
}

void EpdSpi::reset(uint8_t millis=20) {
    gpio_hold_dis((gpio_num_t)CONFIG_EINK_RST);
    gpio_set_level((gpio_num_t)CONFIG_EINK_RST, 0);
    vTaskDelay(millis / portTICK_PERIOD_MS);
    gpio_set_level((gpio_num_t)CONFIG_EINK_RST, 1);
    gpio_hold_en((gpio_num_t)CONFIG_EINK_RST);
    vTaskDelay(millis / portTICK_PERIOD_MS);
}
