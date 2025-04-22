// hc05.h
#ifndef HC05_H
#define HC05_H

void HC05_Init(void);
char HC05_Read(void);
void HC05_Write(char data);
void HC05_WriteString(char *str);

#endif
