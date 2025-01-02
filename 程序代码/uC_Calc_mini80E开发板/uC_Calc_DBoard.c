#include <reg51.H>
#include<intrins.h>
#include<stdio.h>
#include<string.h>
#include<math.h>

sbit RS = P2^4;
sbit RW = P2^5;
sbit E = P2^6;

#define Data P0

float num1;
float num2;
char yun_flag = ' ';
char key_last;
char sqrt_flag;
unsigned long pos_flag = 1;
char neg_flag;
char data c_num1[15] = " ";
char data c_num2[15] = " ";
char error;
char len;

void DelayUs(unsigned char us)
{
    unsigned char uscnt;
    uscnt = us >> 1;
    while (--uscnt)
    {
    }
}

void DelayMs(unsigned char ms)
{
    while (--ms)
    {
        DelayUs(250);
        DelayUs(250);
        DelayUs(250);
        DelayUs(250);
    }
}

void WriteCommand(unsigned char c)
{
    DelayMs(5);
    E = 0;
    RS = 0;
    RW = 0;
    _nop_();
    E = 1;
    Data = c;
    E = 0;
}

void WriteData(unsigned char c)
{
    DelayMs(5);
    E = 0;
    RS = 1;
    RW = 0;
    _nop_();
    E = 1;
    Data = c;
    E = 0;
    RS = 0;
}

void ShowChar(unsigned char pos, unsigned char c)
{
    unsigned char p;
    if (pos >= 0x10)
    {
        p = pos + 0xb0;
    }
    else
    {
        p = pos + 0x80;
    }
    WriteCommand(p);
    WriteData(c);
}

void ShowString(unsigned char line, char *ptr)
{
    unsigned char l, i;
    l = line << 4;
    for (i = 0; *(ptr + i) != '\0'; i++)
    {
        ShowChar(l++, *(ptr + i));
    }
}

void InitLcd(void)
{
    DelayMs(15);
    WriteCommand(0x38);
    WriteCommand(0x38);
    WriteCommand(0x38);
    WriteCommand(0x06);
    WriteCommand(0x0c);
    WriteCommand(0x01);
}

char scan(void)
{
    char h_data, l_data, i, key_num;
    P2 = P2 & 0XE0;
    P1 = P1 | 0X0F;
    while ((P1 | 0xf0) == 0xff)
    {
        while ((P1 | 0xf0) == 0xff)
        {
        }
        DelayMs(10);
    }
    l_data = ~(P1 | 0xf0);
    P2 = P2 | 0X1F;
    P1 = P1 & 0XF0;
    h_data = ~(P2 | 0xe0);
    for (i = 0; i <= 4; i++)
    {
        if (h_data == 1)
        {
            break;
        }
        h_data = h_data >> 1;
    }
    h_data = i;
    for (i = 1; i <= 4; i++)
    {
        if (l_data == 1)
        {
            break;
        }
        l_data = l_data >> 1;
    }
    l_data = i;
    key_num = h_data * 4 + l_data;

    P2 = P2 & 0XE0;
    P1 = P1 | 0X0F;
    while ((P1 | 0xf0) != 0xff)
    {
        while ((P1 | 0xf0) != 0xff)
        {
        }
        DelayMs(10);
    }
    return key_num;
}

void float_to_char(float a, char *p)
{
    char i, flag, length;
    flag = 0;
    sprintf(p, "%f", a);
    length = strlen(p);
    for (i = 0; i < length; i++)
    {
        if (*(p + i) == '.')
        {
            flag = 1;
        }
    }
    if (flag == 1)
    {
        for (i = length - 1; i >= 0; i--)
        {
            if (*(p + i) == '.')
            {
                *(p + i) = '\0';
                break;
            }
            if (*(p + i) != '0')
            {
                *(p + i + 1) = '\0';
                break;
            }
        }
    }
}

void refresh(void)
{
    char length, i, j;
    char dot;
    dot = 0;

    float_to_char(num2, c_num2);
    length = strlen(c_num2);
    for (i = 0, j = 0; i < length; i++)
    {
        if (c_num2[i] == '.')
        {
            dot = 1;
        }
        if (dot == 1)
        {
            j++;
        }
    }
    if (j < len)
    {
        if (dot == 1)
        {
            for (i = length; i < (length + len - j); i++)
            {
                c_num2[i] = '0';
            }
            c_num2[i] = '\0';
        }
        else
        {
            c_num2[length] = '.';
            if (len - j > 1)
            {
                for (i = length + 1; i < (length + len - j); i++)
                {
                    c_num2[i] = '0';
                }
            }
            c_num2[length + len - j] = '\0';
        }
    }

    if (neg_flag == 1)
    {
        length = strlen(c_num2);
        for (i = length - 1; i >= 0; i--)
        {
            c_num2[i + 1] = c_num2[i];
        }
        c_num2[length + 1] = '\0';
        c_num2[0] = '-';
    }

    if (sqrt_flag == 1)
    {
        length = strlen(c_num2);
        for (i = length - 1; i >= 0; i--)
        {
            c_num2[i + 1] = c_num2[i];
        }
        c_num2[length + 1] = '\0';
        c_num2[0] = 0xe8;
    }

    if (error == 0)
    {
        float_to_char(num1, c_num1);
        WriteCommand(0x01);
        ShowString(0, c_num1);
        ShowString(1, c_num2);
        ShowChar(15, yun_flag);
    }
    else
    {
        WriteCommand(0x01);
        ShowString(0, "error!!!");
    }
}

void operation1(char keynum)
{
    if (key_last == 1)
    {
        if (neg_flag == 1)
        {
            num2 = -num2;
        }
        if (sqrt_flag == 1)
        {
            if (num2 >= 0)
            {
                num2 = sqrt(num2);
            }
            else
            {
                error = 1;
            }
        }
        if (yun_flag == '+')
        {
            num1 = num1 + num2;
        }
        if (yun_flag == '-')
        {
            num1 = num1 - num2;
        }
        if (yun_flag == '*')
        {
            num1 = num1 * num2;
        }
        if (yun_flag == '/')
        {
            if (num2 != 0)
            {
                num1 = num1 / num2;
            }
            else
            {
                error = 1;
            }
        }
        if (yun_flag == ' ' && num2 != 0)
        {
            num1 = num2;
        }
        num2 = 0;
        sqrt_flag = 0;
        neg_flag = 0;
        pos_flag = 1;
    }
    if (keynum == 4)
    {
        yun_flag = '+';
    }
    if (keynum == 8)
    {
        yun_flag = '-';
    }
    if (keynum == 12)
    {
        yun_flag = '*';
    }
    if (keynum == 16)
    {
        yun_flag = '/';
    }
    len = 0;
    key_last = 0;
    refresh();
}

void operation2(char keynum)
{
    float Data1;

    if (keynum == 1)
    {
        Data1 = 7;
    }
    if (keynum == 2)
    {
        Data1 = 8;
    }
    if (keynum == 3)
    {
        Data1 = 9;
    }
    if (keynum == 5)
    {
        Data1 = 4;
    }
    if (keynum == 6)
    {
        Data1 = 5;
    }
    if (keynum == 7)
    {
        Data1 = 6;
    }
    if (keynum == 9)
    {
        Data1 = 1;
    }
    if (keynum == 10)
    {
        Data1 = 2;
    }
    if (keynum == 11)
    {
        Data1 = 3;
    }
    if (keynum == 13)
    {
        Data1 = 0;
    }
    if (pos_flag == 1)
    {
        num2 = num2 * 10 + Data1;
    }
    else
    {
        num2 = num2 + (Data1 / pos_flag);
        pos_flag = pos_flag * 10;
        len++;
    }
    key_last = 1;
    refresh();
}

void operation3(void)
{
    if (pos_flag == 1)
    {
        pos_flag = pos_flag * 10;
        len++;
    }
    key_last = 1;
    refresh();
}

void operation4(void)
{
    if (neg_flag == 1)
    {
        num2 = -num2;
    }
    if (sqrt_flag == 1)
    {
        if (num2 >= 0)
        {
            num2 = sqrt(num2);
        }
        else
        {
            error = 1;
        }
    }
    if (yun_flag == '+')
    {
        num1 = num1 + num2;
    }
    if (yun_flag == '-')
    {
        num1 = num1 - num2;
    }
    if (yun_flag == '*')
    {
        num1 = num1 * num2;
    }
    if (yun_flag == '/')
    {
        if (num2 != 0)
        {
            num1 = num1 / num2;
        }
        else
        {
            error = 1;
        }
    }
    if (yun_flag == ' ' && num2 != 0)
    {
        num1 = num2;
    }
    num2 = 0;
    sqrt_flag = 0;
    neg_flag = 0;
    pos_flag = 1;
    yun_flag = ' ';
    len = 0;
    key_last = 0;
    refresh();
}

void operation5(void)
{
    num1 = 0;
    num2 = 0;
    sqrt_flag = 0;
    neg_flag = 0;
    pos_flag = 1;
    yun_flag = ' ';
    error = 0;
    len = 0;
    key_last = 0;
    refresh();
}

void operation6(void)
{
    char i, length;
    if (num2 == 0)
    {
        sqrt_flag = 0;
        neg_flag = 0;
        pos_flag = 1;
        len = 0;
        key_last = 0;
        refresh();
        return;
    }
    sprintf(c_num2, "%f", num2);
    length = strlen(c_num2);
    for (i = length - 1; i >= 0; i--)
    {
        if (c_num2[i] != '0' && c_num2[i] != '.')
        {
            break;
        }
    }
    if (pos_flag == 1)
    {
        num2 -= (c_num2[i] - '0');
        num2 = num2 / 10;
    }
    else
    {
        pos_flag /= 10;
        num2 *= pos_flag;
        num2 -= (c_num2[i] - '0');
        num2 /= pos_flag;
        len--;
    }
    refresh();
}

void operation7(void)
{
    if (neg_flag == 0)
    {
        neg_flag = 1;
    }
    else
    {
        neg_flag = 0;
    }
    key_last = 1;
    refresh();
}

void operation8(void)
{
    if (sqrt_flag == 0)
    {
        sqrt_flag = 1;
    }
    else
    {
        sqrt_flag = 0;
    }
    key_last = 1;
    refresh();
}

void key(char keynum)
{
    switch (keynum)
    {
        case 4 :
        case 8 :
        case 12:
        case 16:
        {
            if (error == 0)
            {
                operation1(keynum);
            }
            break;
        }

        case 1  :
        case 2  :
        case 3  :
        case 5  :
        case 6  :
        case 7  :
        case 9  :
        case 10 :
        case 11 :
        case 13 :
        {
            if (error == 0)
            {
                operation2(keynum);
            }
            break;
        }

        case 14 :
        {
            if (error == 0)
            {
                operation3();
            }
            break;
        }

        case 15 :
        {
            if (error == 0)
            {
                operation4();
            }
            break;
        }

        case 17 :
        {
            operation5();
            break;
        }

        case 18 :
        {
            if (error == 0)
            {
                operation6();
            }
            break;
        }

        case 19 :
        {
            if (error == 0)
            {
                operation7();
            }
            break;
        }

        case 20 :
        {
            if (error == 0)
            {
                operation8();
            }
            break;
        }
    }
}

void main(void)
{
    InitLcd();
    DelayMs(15);
    refresh();
    while (1)
    {
        key(scan());
    }
}
