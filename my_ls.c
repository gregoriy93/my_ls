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
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>


/// @brief Односвязный список каталогов
struct dirname_t
{
    char *dirname;                  ///Имя каталога
    struct dirname_t *next_dir;     ///Указатель на следующий элемент
};


char* str_default_dir = "./";


bool list = false;
bool reverse = false;
bool non_bytes = false;


/**
 * @brief Вывести список файлов в каталогах
 * 
 * @param dir_list Список каталогов
 */
void print_files_list(struct dirname_t *dir_list)
{
    struct dirname_t *dir_iter = dir_list;

    while(1)
    {
        DIR *dir_desc_ptr;
        struct dirent *ep;

        dir_desc_ptr = opendir(dir_iter->dirname);
        if(dir_desc_ptr == NULL)
        {
            printf("No such directory: %s", dir_iter->dirname);
        }

        while((ep = readdir(dir_desc_ptr)) != NULL)
        {
            printf("%s\n", ep->d_name);
        }

        //Достигли конца списка каталогов
        if(dir_iter->next_dir == NULL) break;
        dir_iter = dir_iter->next_dir;
    }
}


int main(int argc, char* argv[])
{
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
    
    //Голова списка каталогов
    struct dirname_t head;
    head.dirname = NULL;
    head.next_dir = NULL;

    //Итератор
    struct dirname_t *dir_iter = &head;

    //Для определения списка заданных каталогов
    for (int i = 1; i < argc; i++)
    {
        if(argv[i][0] != '-')
        {
            dir_iter->next_dir = (struct dirname_t *)malloc(sizeof(struct dirname_t));
            dir_iter = dir_iter->next_dir;
            dir_iter->dirname = NULL;
            dir_iter->next_dir = NULL;

            dir_iter->dirname = (char *)malloc(strlen(argv[i]) * sizeof(char));
            strcpy(dir_iter->dirname, argv[i]);
        }
    }

    //Если каталогов не задали
    if(head.next_dir == NULL)
    {   //Каталог по-умолчанию
        head.dirname = str_default_dir;
    }

    print_files_list(&head);
    
    return 0;
}