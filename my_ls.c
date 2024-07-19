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
#include <sys/stat.h>
#include <sys/types.h>
#include <grp.h>
#include <pwd.h>


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

        //Открытие каталога
        dir_desc_ptr = opendir(dir_iter->dirname);
        if(dir_desc_ptr == NULL)
        {   //Каталог не найден
            printf("No such directory: %s", dir_iter->dirname);
            return;
        }

        //Подсчёт числа файлов в каталоге
        int files_count = 0;
        while((ep = readdir(dir_desc_ptr)) != NULL)
        {
            files_count++;
        }

        //Пустой каталог
        if(files_count == 0)
        {
            printf("./\n");
            printf("../\n");
            return;            
        }

        struct stat file_stat_list[files_count];
        char filename[files_count][PATH_MAX];

        //Итератор
        int file_iterator = 0;

        //Так делать не стоит. Вдруг что-то в каталоге поменяется?
        closedir(dir_desc_ptr);
        dir_desc_ptr = opendir(dir_iter->dirname);
        if(dir_desc_ptr == NULL)
        {   //Каталог не найден
            printf("No such directory: %s", dir_iter->dirname);
            return;
        }        

        //Получение списка файлов со всеми аттрибутами
        while((ep = readdir(dir_desc_ptr)) != NULL)
        {
            char path[PATH_MAX];
            snprintf(path, PATH_MAX, "%s/%s", dir_iter->dirname, ep->d_name);
            if(stat(path, &file_stat_list[file_iterator])) 
            {
                printf("stat failed\n");
                return;
            }

            strcpy(filename[file_iterator], ep->d_name);

            file_iterator++;
        }

        file_iterator = 0;
        //Добавление
        int file_addin = 1;

        //Для реверсивного вывода меняем порядок обхода на обратный
        if(reverse)
        {
            file_iterator = files_count - 1;
            file_addin = -1;
        }

        //Вывод содержимого папки
        for (int i = 0; i < files_count; i++)
        {
            if(list)
            {
                printf( (S_ISDIR(file_stat_list[file_iterator].st_mode)) ? "d" : "-");
                printf( (file_stat_list[file_iterator].st_mode & S_IRUSR) ? "r" : "-");
                printf( (file_stat_list[file_iterator].st_mode & S_IWUSR) ? "w" : "-");
                printf( (file_stat_list[file_iterator].st_mode & S_IXUSR) ? "x" : "-");
                printf( (file_stat_list[file_iterator].st_mode & S_IRGRP) ? "r" : "-");
                printf( (file_stat_list[file_iterator].st_mode & S_IWGRP) ? "w" : "-");
                printf( (file_stat_list[file_iterator].st_mode & S_IXGRP) ? "x" : "-");
                printf( (file_stat_list[file_iterator].st_mode & S_IROTH) ? "r" : "-");
                printf( (file_stat_list[file_iterator].st_mode & S_IWOTH) ? "w" : "-");
                printf( (file_stat_list[file_iterator].st_mode & S_IXOTH) ? "x" : "-");
                struct passwd *pwd;
                struct group *grp;

                pwd = getpwuid(file_stat_list[file_iterator].st_uid);
                if (pwd == NULL) {
                    perror("getpwuid");
                    exit(EXIT_FAILURE);
                }
                printf(" %s\n", pwd->pw_name);

                grp = getgrgid(file_stat_list[file_iterator].st_gid);
                if (grp == NULL) {
                    perror("getgrgid");
                    exit(EXIT_FAILURE);
                }
                printf(" %s\n", grp->gr_name);
                printf(" %s\n", filename[file_iterator]);
            }
            else
            {
                //Простой вывод
                printf("%s\n", filename[file_iterator]);
            }

            //Переход к следующему файлу
            file_iterator += file_addin;
        }

        //Достигли конца списка каталогов
        if(dir_iter->next_dir == NULL) break;
        dir_iter = dir_iter->next_dir;
    }
}


/**
 * @brief Главная функция
 * 
 * @param argc Число параметров
 * @param argv Массив параметров
 * @return int Возвращаемый результат
 */
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

        if(optind >= argc) break;
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