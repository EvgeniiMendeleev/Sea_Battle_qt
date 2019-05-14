#include <stdio.h>

struct Table
{
    int tmpBuffer[10][10] = {};
};

void set_on_Table(Table* table, int i, int j, int typeOfship)
{
    table->tmpBuffer[i][j] = typeOfship;
}

void take_on_Table(Table* table, int i, int j)
{
    return table->tmpBuffer[i][j];
}
