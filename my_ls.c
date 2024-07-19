/**
 * @file my_ls.c
 * @author Gregoriy Bereznyakov (gregoriy93.gb@gmail.com)
 * @brief Консольное приложение-аналог утилиты ls для тестового задания
 * @date 2024-07-19
 * 
 * @copyright Square Enix (c) 2024
 * 
 */


#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>


int main(int argc, char* argv[])
{
    bool list = false, reverse = false, non_bytes = false;

    //Определение входных параметров
    while(1)
    {
        int param = getopt(argc, argv, "lhr");

        //Если был задан параметр, начинающийся на "-"
        if(param != -1)
        {
            switch(param)
            {
                //выводит подробную таблицу файлов в директории
                case 'l':
                list = true;
                break;

                //изменяет формат вывода размеров файлов. Вместо размера в байтах
                case 'h':
                non_bytes = true;
                break;

                //изменяет порядок вывода файлов на обратный
                case 'r':
                reverse = true;
                break;
            }
        }
        else
        {
            //Это что-то другое (имя каталога)
            optind++;
        }

        if(optind == argc) break;
    }

    //Для определения списка заданных каталогов
    for (int i = 1; i < argc; i++)
    {
        if(argv[i][0] != '-')
        {
            printf("size: %d \n", strlen(argv[i]));
        }
    }
    
    
    return 0;
}