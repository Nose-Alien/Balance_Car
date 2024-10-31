
#include "driver_ssd1306.h"
#include "oledfont.h"

#include "i2c.h"
//#include "spi.h"
#include "driver_i2c.h"

#include "gpio.h"
#include "stdio.h"
//#include "stdlib.h"


unsigned char OLED_GRAM[128][8];


//I2C控制
void OLED_Write_Byte(uint8_t dat, uint8_t cmd)
{
    if(cmd == 0) {
// I2C句柄, OLED屏幕的I2C地址, OLED屏幕的命令字节, 内存地址大小为8位, 要写入的数据, 要写入的数据长度, 超时时间（毫秒）
        HAL_I2C_Mem_Write(&WHICH_I2C, OLED0561_ADD, I2C_CMD,
                          I2C_MEMADD_SIZE_8BIT, &dat, 1, 0xffff);
//        I2C_Start();
//        I2C_WriteByte(OLED0561_ADD);
//        // I2C_WriteByte(OLED_ADDR | OLED_WRITE);
//        I2C_WriteByte(I2C_CMD);
//        I2C_WriteByte(dat);
//        I2C_Stop();
    } else if(cmd == 1){
        HAL_I2C_Mem_Write(&WHICH_I2C, OLED0561_ADD, I2C_DATA,
                          I2C_MEMADD_SIZE_8BIT, &dat, 1, 0xffff);
//        I2C_Start();
//        I2C_WriteByte(OLED0561_ADD);
////         I2C_WriteByte(OLED_ADDR | OLED_WRITE);
//        I2C_WriteByte(I2C_DATA);
//        I2C_WriteByte(dat);
//        I2C_Stop();
    }
}

/*
 *  函数名：OLED_WriteNBytes
 *  功能描述：连续发送N个写入显存的数据：开始信号->设备地址->控制字节->N个GDRAM数据->停止信号
 *  输入参数：buf——>指向保存了要发送数据的首地址；length-->发送数据的个数
 *  输出参数：无
 *  返回值：无
*/
//void OLED_WriteNBytes(uint8_t *buf, uint16_t length)
//{
//    uint16_t i = 0;
//    if(buf == NULL) return;
//    I2C_Start();
//    I2C_WriteByte(OLED0561_ADD);
//    I2C_WriteByte(0x40);
//    for(i=0; i<length; i++)
//    {
//        I2C_WriteByte(buf[i]);
//    }
//    I2C_Stop();
//}


//OLED初始化
void OLED_Init(void)
{
    HAL_Delay(200); //这里的延时很重要
    OLED_Write_Byte(0xAE, Write_CMD);  //显示关闭
    OLED_Write_Byte(0x00, Write_CMD);  //设置低列地址
    OLED_Write_Byte(0x10, Write_CMD);  //设置高列地址
    OLED_Write_Byte(0x40, Write_CMD);  //设置显示开始行 0x63
    OLED_Write_Byte(0x81, Write_CMD);  //设置对比度
    OLED_Write_Byte(0xcf, Write_CMD);  //亮度调节 0x00~0xff

    OLED_Write_Byte(0xA6, Write_CMD);

    OLED_Write_Byte(0xA8, Write_CMD);  //设置驱动路数
    OLED_Write_Byte(0x3F, Write_CMD);  //128*64:0x3f   128*32:0x1f

    OLED_Write_Byte(0xD3, Write_CMD);  //设置显示偏移
    OLED_Write_Byte(0x00, Write_CMD);  //默认设置00没有偏移
    OLED_Write_Byte(0xD5, Write_CMD);  //设置显示时钟分频/振荡器频率(刷新率)
    OLED_Write_Byte(0xF0, Write_CMD);  //0x10-0xf0 越大速度越快
    OLED_Write_Byte(0xD9, Write_CMD);  //--设置预充电时期
    OLED_Write_Byte(0xF1, Write_CMD);  //
    OLED_Write_Byte(0xDA, Write_CMD);  //设置COM硬件引脚配置，适应分辨率
    OLED_Write_Byte(0x12, Write_CMD);  //128*64:0x12   128*32:0x02
    OLED_Write_Byte(0xDB, Write_CMD);  //设置vcomn 电压倍率
    OLED_Write_Byte(0x40, Write_CMD);  //0x20,0.77xVcc

    OLED_Write_Byte(0x20, Write_CMD);  //设置内存地址模式 3种 水平/垂直/页寻址(默认)
    OLED_Write_Byte(0x02, Write_CMD);  //水平0x00 / 垂直0x01 / 页寻址0x02
    OLED_Write_Byte(0xb0, Write_CMD);  //为页寻址模式设置页面开启地址0-7

    OLED_Write_Byte(0x8D, Write_CMD);  //充电泵设置
    OLED_Write_Byte(0x14, Write_CMD);  //bit2开启/关闭  允许在显示开启的时候使用  0x10：不允许在显示开启前使用
    OLED_Write_Byte(0xA4, Write_CMD);  //0xa4输出遵循RAM内容 0x50输出忽略RAM内容

    OLED_Write_Byte(0xA6, Write_CMD);  //正常显示:0xa6 反相显示:0xa7
    OLED_Write_Byte(0xA1, Write_CMD);  //(设置镜像，A0左右镜像，A1正常显示)对于IIC通讯4引脚OLED要设置0xa1
    OLED_Write_Byte(0xC8, Write_CMD);  //(设置镜像，C0左右镜像，C8正常显示)设置COM扫描方式
    OLED_Write_Byte(0xAF, Write_CMD);  //显示开启
//    OLED_Fill(0x00);  //全部填充0x00
}

//void OLED_SetPos(unsigned char x, unsigned char y) //设置起始点坐标
//{
//	OLED_Write_Byte(0xb0+y, Write_CMD);
//	OLED_Write_Byte(x & 0x0f, Write_CMD);
//	OLED_Write_Byte(((x & 0xf0) >> 4) | 0x10, Write_CMD);
//}
/**
 * @brief  清屏
 * @param  backColor 背景色
 */
void OLED_Clear(uint32_t backColor)
{
    uint8_t x, y;

    if(backColor == 0xffff) {
        OLED_Fill(0xffff);
    } else {
        OLED_Fill(0x0000);
    }
}

/**
 * @brief  全屏填充
 * @param  fill_Data
 */
void OLED_Fill(uint32_t fill_Data)
{
    uint8_t x, y;

    for(y=0;y<8;y++) {
        for(x=0;x<128;x++) {
            OLED_GRAM[x][y] = fill_Data;
        }
    }
//	OLED_Refresh_Gram();
}

/**
 * @brief  更新显存到oled
 */
void OLED_Refresh_Gram(void)
{
    uint8_t x, y;
    OLED_Write_Byte(0x00, Write_CMD);		//设置显示位置—列低地址
    OLED_Write_Byte(0x10, Write_CMD);		//设置显示位置—列高地址
    for(y=0;y<8;y++) {  //循环8次，就会刷新8页
        OLED_Write_Byte(0xb0 + y, Write_CMD);   //设置页地址（0~7）
        for(x=0;x<128;x++) {
            OLED_Write_Byte(OLED_GRAM[x][y], Write_DATA);
        }
    }
}

//开启OLED显示
void OLED_DisPlay_On(void)
{
    OLED_Write_Byte(0x8D,Write_CMD);//电荷泵使能
    OLED_Write_Byte(0x14,Write_CMD);//开启电荷泵
    OLED_Write_Byte(0xAF,Write_CMD);//点亮屏幕
}

//关闭OLED显示
void OLED_DisPlay_Off(void)
{
    OLED_Write_Byte(0x8D,Write_CMD);//电荷泵使能
    OLED_Write_Byte(0x10,Write_CMD);//关闭电荷泵
    OLED_Write_Byte(0xAF,Write_CMD);//关闭屏幕
}

//反显函数
void OLED_ColorTurn(uint8_t i)
{
    if(i==0)
    {
        OLED_Write_Byte(0xA6,Write_CMD);//正常显示
    }
    if(i==1)
    {
        OLED_Write_Byte(0xA7,Write_CMD);//反色显示
    }
}
//屏幕旋转180度
void OLED_DisplayTurn(uint8_t i)
{
    if(i==0)
    {
        OLED_Write_Byte(0xC8,Write_CMD);//正常显示
        OLED_Write_Byte(0xA1,Write_CMD);
    }
    if(i==1)
    {
        OLED_Write_Byte(0xC0,Write_CMD);//反转显示
        OLED_Write_Byte(0xA0,Write_CMD);
    }
}



/**
 * @brief  画点
 * @param  x 0-127
 * @param  y 0-63
 * @param  color 0 or 1
 */
int16_t OLED_DrawPoint(uint16_t x, uint16_t y, uint32_t color)
{
    uint8_t pageIndex, rowOffset , bitMask;

    if(x>127 || x<0 || y>63 || y<0)
        return -1;

    if(color == 0xffff) {
        pageIndex = y / 8;    // 计算所在页码
        rowOffset = y % 8;  // 计算所在页的偏移量
        bitMask = 1 << rowOffset;  // 用于设置像素的位掩码
        OLED_GRAM[x][pageIndex] |= bitMask;  // 将对应位置的像素点设置为亮点
    } else if(color == 0x0000) {
        pageIndex = y / 8;    // 计算所在页码
        rowOffset = y % 8;  // 计算所在页的偏移量
        bitMask = 1 << rowOffset;  // 用于设置像素的位掩码
        OLED_GRAM[x][pageIndex] = ~OLED_GRAM[x][pageIndex];   // 取反
        OLED_GRAM[x][pageIndex] |= bitMask;                   // 置1
        OLED_GRAM[x][pageIndex] = ~OLED_GRAM[x][pageIndex];   // 再取反，确保该点置0
    } else {
        return -1;
    }
}



//画线
//x:0~127
//y:0~63
void OLED_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
//Bresenham直线算法
    int8_t dx = x2 - x1;  //水平和垂直方向 坐标差异
    int8_t dy = y2 - y1;

    int8_t stepX = (dx > 0) ? 1 : -1;  //水平和垂直方向 步进方向
    int8_t stepY = (dy > 0) ? 1 : -1;

    dx = dx > 0 ? dx : -dx;  //绝对值 确定在水平和垂直方向上需要进行多少个步骤
    dy = dy > 0 ? dy : -dy;

    int8_t error = (dx > dy ? dx : -dy) / 2;  //跟踪当前像素位置与理想直线路径之间的误差

    while (1) {
        OLED_DrawPoint(x1, y1, 0xffff);

        if (x1 == x2 && y1 == y2) {
            break;
        }

        int8_t e2 = error;

        if (e2 > -dx) {  //误差超过了一定阈值，我们会将 error 进行相应的调整
            error -= dy;
            x1 += stepX;
        }

        if (e2 < dy) {
            error += dx;
            y1 += stepY;
        }
    }
}

//x,y:圆心坐标
//r:圆的半径
void OLED_DrawCircle(uint8_t centerX, uint8_t centerY, uint8_t radius)
{
    int8_t x = radius;
    int8_t y = 0;
    int8_t err = 0;

    while (x >= y) {
        OLED_DrawPoint(centerX + x, centerY + y, 0xffff);
        OLED_DrawPoint(centerX + y, centerY + x, 0xffff);
        OLED_DrawPoint(centerX - y, centerY + x, 0xffff);
        OLED_DrawPoint(centerX - x, centerY + y, 0xffff);
        OLED_DrawPoint(centerX - x, centerY - y, 0xffff);
        OLED_DrawPoint(centerX - y, centerY - x, 0xffff);
        OLED_DrawPoint(centerX + y, centerY - x, 0xffff);
        OLED_DrawPoint(centerX + x, centerY - y, 0xffff);

        if (err <= 0) {
            y += 1;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}

//画矩形;x:0~127 y:0~63
//x1,y1,左上角坐标；x2,y2,右下角坐标
void OLED_DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
    uint8_t x, y;

    for (x = x1; x <= x2; x++) {
        OLED_DrawPoint(x, y1, 0xffff);
        OLED_DrawPoint(x, y2, 0xffff);
    }

    for (y = y1; y <= y2; y++) {
        OLED_DrawPoint(x1, y, 0xffff);
        OLED_DrawPoint(x2, y, 0xffff);
    }
}

//画一个字符;x:0~127 y:0~63
//fontSize: SMALL, MEDIUM, BIG
//注意使用到的字库，取模方式不一样
void OLED_DrawChar(uint8_t x, uint8_t y, uint8_t ch, enum Font_Size fontSize,uint8_t Color_Turn)
{
    const uint8_t *charData;
    uint8_t row, col;

    switch(fontSize) {
        case SMALL:
            //8行6列字模
            charData = F8x6[ch - ' '];
            if (Color_Turn == 0) {
                for (col = 0; col < 6; col++) {
                    for (row = 0; row < 8; row++) {
                        if (charData[col] & (1 << row)) {
                            OLED_DrawPoint(x + col, y + row, 0xffff);
                        }
                    }
                }
            } else if (Color_Turn == 1) {
                for (col = 0; col < 6; col++) {
                    for (row = 0; row < 8; row++) {
                        if (~charData[col] & (1 << row)) {
                            OLED_DrawPoint(x + col, y + row, 0xffff);
                        }
                    }
                }
            }
            break;
        case MEDIUM:
            //16行8列字模
            charData = F16x8[ch - ' '];
            if (Color_Turn == 0) {
                for (col = 0; col < 8; col++) {
                    for (row = 0; row < 16; row++) {
                        if (row < 8 && (charData[col] & (1 << row))) {  //处理第一页
                            OLED_DrawPoint(x + col, y + row, 0xffff);
                        } else if (row >= 8 && (charData[col + 8] & (1 << (row - 8)))) {  //处理第二页
                            OLED_DrawPoint(x + col, y + row, 0xffff);
                        }
                    }
                }
            } else if (Color_Turn == 1)
            {
                for (col = 0; col < 8; col++) {
                    for (row = 0; row < 16; row++) {
                        if (row < 8 && (~charData[col] & (1 << row))) {  //处理第一页
                            OLED_DrawPoint(x + col, y + row, 0xffff);
                        } else if (row >= 8 && (~charData[col + 8] & (1 << (row - 8)))) {  //处理第二页
                            OLED_DrawPoint(x + col, y + row, 0xffff);
                        }
                    }
                }
            }
            break;
        case BIG:
            //32行16列字模
            charData = F32x16[ch - ' '];
            if (Color_Turn == 0) {
                for (col = 0; col < 16; col++) {
                    for (row = 0; row < 32; row++) {

                        if (row < 8 && (charData[col] & (1 << row))) {  //第一页，字模数据在[0-16]
                            OLED_DrawPoint(x + col, y + row, 0xffff);

                        } else if (row >= 8 && row < 16 &&
                                   (charData[col + 16] & (1 << (row - 8)))) {  //第二页，字模数据在[16-32]
                            OLED_DrawPoint(x + col, y + row, 0xffff);

                        } else if (row >= 16 && row < 24 &&
                                   (charData[col + 32] & (1 << (row - 16)))) {  //第三页，字模数据在[32-48]
                            OLED_DrawPoint(x + col, y + row, 0xffff);

                        } else if (row >= 24 && row < 32 &&
                                   (charData[col + 48] & (1 << (row - 24)))) {  //第四页，字模数据在[48-64]
                            OLED_DrawPoint(x + col, y + row, 0xffff);

                        }
                    }
                }
            }else if (Color_Turn == 1)
            {
                for (col = 0; col < 16; col++) {
                    for (row = 0; row < 32; row++) {

                        if (row < 8 && (~charData[col] & (1 << row))) {  //第一页，字模数据在[0-16]
                            OLED_DrawPoint(x + col, y + row, 0xffff);

                        } else if (row >= 8 && row < 16 &&
                                   (~charData[col + 16] & (1 << (row - 8)))) {  //第二页，字模数据在[16-32]
                            OLED_DrawPoint(x + col, y + row, 0xffff);

                        } else if (row >= 16 && row < 24 &&
                                   (~charData[col + 32] & (1 << (row - 16)))) {  //第三页，字模数据在[32-48]
                            OLED_DrawPoint(x + col, y + row, 0xffff);

                        } else if (row >= 24 && row < 32 &&
                                   (~charData[col + 48] & (1 << (row - 24)))) {  //第四页，字模数据在[48-64]
                            OLED_DrawPoint(x + col, y + row, 0xffff);

                        }
                    }
                }
            }
            break;
        default:
            return;
    }

}

//画字符串;x:0~127 y:0~63
//fontSize: SMALL, MEDIUM, BIG
//注意使用到的字库，取模方式不一样
void OLED_DrawStr(uint8_t x, uint8_t y, uint8_t * ch, enum Font_Size fontSize, uint16_t Color_Turn)
{
    while((*ch>=' ')&&(*ch<='~'))//判断是不是非法字符!
    {
        OLED_DrawChar(x,y,*ch,fontSize,Color_Turn);
        switch(fontSize) {
            case SMALL:
                x += 6;
                if(x > 128-6) {
                    x = 0;
                    y += 8;
                }
                break;
            case MEDIUM:
                x += 8;
                if(x > 128-8) {
                    x = 0;
                    y += 16;
                }

                break;
            case BIG:
                x += 16;
                if(x > 128-16) {
                    x = 0;
                    y += 32;
                }
                break;
            default:
                return;
        }
        ch++;
    }
}



// 10的次方
uint16_t Ten_Exponent(uint8_t exp)
{
    uint16_t result = 1;

    while(exp--) {
        result *= 10;
    }
    return result;
}


// 显示数字(最大5位十进制）
// x: 0~127, y: 0~63, num: 0~65535
// fontSize: SMALL, MEDIUM, BIG
void OLED_DrawNum(uint8_t x, uint8_t y, uint16_t num, enum Font_Size fontSize, uint16_t Color_Turn) {
    uint16_t num1;
    uint8_t temp[5] = {0};  // 用于存储转换后的数字字符
    uint8_t i, x1, fontLen, numLen = 0;

    // 计算数字的位数
    num1 = num;
    do {
        num1 /= 10;
        numLen++;
    } while (num1 != 0);

    // 根据字体大小选择字符宽度
    switch (fontSize) {
        case SMALL:
            fontLen = 6;
            break;
        case MEDIUM:
            fontLen = 8;
            break;
        case BIG:
            fontLen = 16;
            break;
        default:
            return;
    }

    // 将数字转换为字符数组，从最低位开始处理
    for (i = 0; i < numLen; i++) {
        temp[numLen - 1 - i] = (num / Ten_Exponent(i)) % 10 + '0';
    }

    // 根据字体大小和字符长度调整显示位置
    for (i = 0; i < numLen; i++) {
        x1 = i * fontLen;  // 根据字体大小、位数调整显示位置
        OLED_DrawChar(x + x1, y, temp[i], fontSize,Color_Turn);
    }
}


////显示数字(最大5位十进制）
////x:0~127 y:0~63 num:0~65535
////fontSize: SMALL, MEDIUM, BIG
void OLED_DrawNum0(uint8_t x, uint8_t y, uint16_t num, enum Font_Size fontSize, uint16_t Color_Turn) {
    uint8_t temp[5] = {0};  // 用于存储转换后的数字字符
    uint8_t i, x1, fontLen, numLen = 0;

    // 根据字体大小选择字符宽度
    switch (fontSize) {
        case SMALL:
            fontLen = 6;
            break;
        case MEDIUM:
            fontLen = 8;
            break;
        case BIG:
            fontLen = 16;
            break;
        default:
            return;
    }

    // 判断输入的数字位数，如果小于10，强制设置为两位数格式
    if (num < 10) {
        // 如果num小于10，手动加一个前导0
        temp[0] = '0';
        temp[1] = num + '0';
        numLen = 2;  // 强制两位数显示
    } else {
        // 将数字转换为字符数组，从最低位开始处理
        uint16_t num1 = num;
        do {
            temp[numLen++] = (num1 % 10) + '0';  // 将数字从右到左存入数组
            num1 /= 10;
        } while (num1 != 0);

        // 翻转字符顺序，使数字按正常顺序显示
        for (i = 0; i < numLen / 2; i++) {
            uint8_t tempChar = temp[i];
            temp[i] = temp[numLen - 1 - i];
            temp[numLen - 1 - i] = tempChar;
        }
    }

    // 根据字体大小和字符长度调整显示位置
    for (i = 0; i < numLen; i++) {
        x1 = i * fontLen;  // 根据字体大小、位数调整显示位置
        OLED_DrawChar(x + x1, y, temp[i], fontSize,Color_Turn);
    }
}

/**
  * @brief  OLED次方函数
  * @retval 返回值等于X的Y次方
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;
    while (Y--)
    {
        Result *= X;
    }
    return Result;
}

/**
  * @brief  OLED显示数字（十进制，带符号数）
  * @param  Line 起始行位置，范围：0~127
  * @param  Column 起始列位置，范围：0~63
  * @param  Number 要显示的数字，范围：-2147483648~2147483647
  * @fontSize  fontSize SMALL, MEDIUM, BIG
  * @retval 无
  */

void OLED_ShowSignedNum(uint8_t x, uint8_t y, int32_t Number,enum Font_Size fontSize,uint16_t Color_Turn)
{
    uint8_t i;
    uint32_t Number1;
    if (Number >= 0)
    {
        OLED_DrawChar(x, y, '+',fontSize,Color_Turn);
        Number1 = Number;
    }
    else
    {
        OLED_DrawChar(x, y, '-',fontSize,Color_Turn);
        Number1 = -Number;
    }
    OLED_DrawNum0(x+8, y, Number1,fontSize,Color_Turn);
}




// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~63); N:汉字在.h中的索引
// Description    : 显示汉字,16*16点阵
//void OLED_DrawCN(uint8_t x, uint8_t y, uint16_t N)
//{
//    const uint8_t *charData;
//	uint8_t row, col, i;
//
//    charData = Hzk2[N];
//
//    for (col = 0; col < 128; col++) {  //有16列
//        for (row = 0; row < 128; row++) {  //有16行
//			if (row < 64 && (charData[col] & (8 << row))) {  //处理第一页(一页是8行)
//                OLED_DrawPoint(x + col, y + row, 0xffff);
//
//            } else if (row >= 64 && (charData[col + 128] & (8 << (row - 64)))) {  //处理第二页
//                OLED_DrawPoint(x + col, y + row, 0xffff);
//
//            }
//        }
//    }
//}

// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~63); N:汉字在.h中的索引
// Description    : 显示汉字,16*16点阵
void OLED_DrawCN(uint8_t x, uint8_t y, uint16_t N, uint16_t Color_Turn)
{
    const uint8_t *charData;
    uint8_t row, col, i;

    charData = Hzk1[N];
    if(Color_Turn==0) {
        for (col = 0; col < 16; col++) {  //有16列
            for (row = 0; row < 16; row++) {  //有16行
                if (row < 8 && (charData[col] & (1 << row))) {  //处理第一页(一页是8行)
                    OLED_DrawPoint(x + col, y + row, 0xffff);

                } else if (row >= 8 && (charData[col + 16] & (1 << (row - 8)))) {  //处理第二页
                    OLED_DrawPoint(x + col, y + row, 0xffff);

                }
            }
        }
    }else if(Color_Turn==1) {
        for (col = 0; col < 16; col++) {  //有16列
            for (row = 0; row < 16; row++) {  //有16行
                if (row < 8 && (~charData[col] & (1 << row))) {  //处理第一页(一页是8行)
                    OLED_DrawPoint(x + col, y + row, 0xffff);
                } else if (row >= 8 && (~charData[col + 16] & (1 << (row - 8)))) {  //处理第二页
                    OLED_DrawPoint(x + col, y + row, 0xffff);

                }
            }
        }
    }
}


// Parameters: x0,y0 -- 起始点坐标(x0:0~127, y0:0~7); x1,y1 -- 起点对角线(结束点)的坐标(x1:1~128,y1:1~8)
// Description: 显示BMP位图
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, unsigned char *BMP)
{
    uint16_t j=0;
    uint8_t x=0,y=0;


    for(y=0; y<8; y++) {  //循环8次，就会刷新8页
        OLED_Write_Byte(0xb0 + y, Write_CMD);   //设置页地址（0~7）
        OLED_Write_Byte(0x00, Write_CMD);		//设置显示位置—列低地址
        OLED_Write_Byte(0x10, Write_CMD);		//设置显示位置—列高地址
        for(x=x0; x<x1; x++) {
            OLED_Write_Byte(BMP1[j], Write_DATA);
            j++;
        }
    }
}

void OLED_Text()
{
    uint32_t num = 0;
    for(num=0; num<=65535; num+=1) {
        OLED_Clear(0x00);
        OLED_DrawNum(0, 0, num, SMALL,0);
        OLED_DrawNum(0, 8, num, MEDIUM,0);
        OLED_DrawNum(0, 24, num, BIG,0);
        OLED_Refresh_Gram();
    }
//    OLED_Fill(0x00);

//    OLED_DrawCN(0*16, 0*16, 0);
//    OLED_DrawCN(0*16, 1*16, 1);
//    OLED_DrawCN(1*16, 0*16, 2);
//    OLED_DrawCN(1*16, 1*16, 3);
//    OLED_DrawCN(2*16, 0*16, 4);
//    OLED_DrawCN(2*16, 1*16, 5);
//    OLED_DrawCN(3*16, 0*16, 6);
//    OLED_DrawCN(3*16, 1*16, 7);
//    OLED_DrawCN(4*16, 0*16, 8);
//    OLED_DrawCN(4*16, 1*16, 9);
//    OLED_DrawCN(5*16, 0*16, 10);
//    OLED_DrawBMP(0, 0, 128, 64, (unsigned char *)BMP1);
//    HAL_Delay(1000);
//    OLED_Refresh_Gram();

}