#include <stdio.h>
#include <string.h>
#include <cstring>
#include <time.h>
#include <stdlib.h>
#ifdef _WIN32
    #include <direct.h>
    #include <io.h>
    #define mkdir(dir, mode) _mkdir(dir)
    #define rmdir(dir) _rmdir(dir)
#else
    #include <sys/stat.h>
    #include <unistd.h>
    #include <dirent.h>
#endif


int loc = 0;
int notes_mode = 0;
char notes_name[64] = "notes.txt";
char notes_path[128];

void get_current_path(int current_loc, const char* filename, char* out_path) {
    switch(current_loc) {
        case 1: sprintf(out_path, "home/%s", filename); break;
        case 2: sprintf(out_path, "documents/%s", filename); break;
        case 3: sprintf(out_path, "downloads/%s", filename); break;
        case 4: sprintf(out_path, "system/%s", filename); break;
        default: sprintf(out_path, "%s", filename); break; 
    }
}

void list_documents_contents() {
#ifdef _WIN32
    struct _finddata_t c_file;
    intptr_t hFile;

    if ((hFile = _findfirst("documents/*", &c_file)) != -1L) {
        do {
            if (strcmp(c_file.name, ".") != 0 && strcmp(c_file.name, "..") != 0) {
                if (c_file.attrib & _A_SUBDIR) {
                    printf("  └─ documents/%s/\n", c_file.name);
                } else {
                    printf("  └─ documents/%s\n", c_file.name);
                }
            }
        } while (_findnext(hFile, &c_file) == 0);
        _findclose(hFile);
    }
#else
    DIR *d = opendir("documents");
    struct dirent *dir;
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                if (dir->d_type == DT_DIR) {
                    printf("  └─ documents/%s/\n", dir->d_name);
                } else {
                    printf("  └─ documents/%s\n", dir->d_name);
                }
            }
        }
        closedir(d);
    }
#endif
}


void process_system_command(char *input) {
    time_t currentTime;
    char content[256];
    FILE *file;

    input[strcspn(input, "\n")] = 0;
    if (strlen(input) == 0) return;

    char cmd[20] = "";
    char arg1[64] = "";
    char arg2[64] = "";
    int parsed_args = sscanf(input, "%s %s %s", cmd, arg1, arg2);

    if (strcmp(cmd, "chmod") == 0) {
        if (parsed_args < 3) {
            printf("err: 0, 1 only found\n");
        } else if (strcmp(arg1, notes_name) != 0) {
            printf("err: file missing\n");
        } else {
            notes_mode = atoi(arg2);
            printf("changed\n");
        }
    }
    else if (strcmp(cmd, "rm") == 0) {
        if (parsed_args < 2) {
            printf("err: missing filename\n");
        } else {
            char target_path[128];
            get_current_path(loc, arg1, target_path);

            if (remove(target_path) == 0) {
                if (strcmp(notes_name, arg1) == 0) {
                    strcpy(notes_name, "notes.txt");
                    notes_mode = 0;
                }
                printf("changed\n");
            } else {
                printf("err: file not found\n");
            }
        }
    }
    else if (strcmp(cmd, "save") == 0) {
        if (parsed_args < 2) {
            printf("err: missing URL or Script file\n");
        } else {
            if (strstr(arg1, ".seal") != NULL) {
                printf("Script %s configuration saved successfully.\n", arg1);
            } else {
                printf("Connecting to remote server...\n");
                printf("Downloading resources from: %s\n", arg1);
                
                if (strstr(arg1, "codepad.app/pad/822052z5n") != NULL) {
                    FILE *dl = fopen("downloads/flag.txt", "w");
                    if (dl) {
                        fprintf(dl, "SEAL{c_strings_are_pointers_to_fun}\n");
                        fclose(dl);
                        printf("Saved successfully to downloads/flag.txt\n");
                    }
                } else {
                    FILE *dl = fopen("downloads/index.html", "w");
                    if (dl) {
                        fprintf(dl, "\n<h1>SealOS Sandbox Landing</h1>\n");
                        fclose(dl);
                        printf("Saved successfully to downloads/index.html\n");
                    }
                }
            }
        }
    }
 
    else if (strcmp(cmd, "exe") == 0) {
        if (parsed_args < 2) {
            printf("err: missing script file\n");
        } else {
            char script_path[128];
            get_current_path(loc, arg1, script_path);
            
            FILE *script = fopen(script_path, "r");
            if (!script) {
                printf("err: script file not found\n");
            } else {
                char script_line[100];
                printf("---  %s ---\n", arg1);
                while (fgets(script_line, sizeof(script_line), script) != NULL) {
                    
                    script_line[strcspn(script_line, "\r\n")] = 0;
                    if (strlen(script_line) == 0) continue; 
                    
                    printf("[%s] executing: %s\n", arg1, script_line);
                    process_system_command(script_line); 
                }
                fclose(script);
                
            }
        }
    }
    else if (strcmp(cmd, "rnm") == 0) { 
        if (parsed_args < 2) {
            printf("err: missing filename\n");
        } else {
            char old_path[128], new_path[128], new_name[64];
            sprintf(old_path, "documents/%s", arg1);
            
            FILE *check = fopen(old_path, "r");
            if (!check) {
                printf("err: file missing\n");
            } else {
                fclose(check);
                printf("rename: ");
                fflush(stdout);
                if (fgets(new_name, sizeof(new_name), stdin) != NULL) {
                    new_name[strcspn(new_name, "\n")] = 0;
                    sprintf(new_path, "documents/%s", new_name);

                    if (rename(old_path, new_path) == 0) {
                        if (strcmp(notes_name, arg1) == 0) {
                            strcpy(notes_name, new_name);
                        }
                        printf("changed\n");
                    } else {
                        printf("err: rename unfound\n");
                    }
                }
            }
        }
    }
    else if (strcmp(cmd, "mkfile") == 0) { 
        if (parsed_args < 2) {
            printf("err: missing name\n");
        } else {
            char folder_path[128];
            sprintf(folder_path, "documents/%s", arg1);
            
            if (mkdir(folder_path, 0777) == 0) {
                printf("changed\n");
            } else {
                printf("err: folder unable to generate\n");
            }
        }
    }
    else if (strcmp(cmd, "ls-d") == 0) {
        printf("prwxf---o---surwx, system/sys.txt\n");
#ifdef _WIN32
        struct _finddata_t c_file;
        intptr_t hFile;
        if ((hFile = _findfirst("documents/*", &c_file)) != -1L) {
            do {
                if (strcmp(c_file.name, ".") != 0 && strcmp(c_file.name, "..") != 0) {
                    if (c_file.attrib & _A_SUBDIR) {
                        printf("drwxf---o---surwx, documents/%s/\n", c_file.name);
                    } else {
                        if (notes_mode == 1) printf("prwxf---o---surwx, documents/%s\n", c_file.name);
                        else printf("p---f---o---surwx, documents/%s\n", c_file.name);
                    }
                }
            } while (_findnext(hFile, &c_file) == 0);
            _findclose(hFile);
        }
#else
        DIR *d = opendir("documents");
        struct dirent *dir;
        if (d) {
            while ((dir = readdir(d)) != NULL) {
                if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                    if (dir->d_type == DT_DIR) {
                        printf("drwxf---o---surwx, documents/%s/\n", dir->d_name);
                    } else {
                        if (notes_mode == 1) printf("prwxf---o---surwx, documents/%s\n", dir->d_name);
                        else printf("p---f---o---surwx, documents/%s\n", dir->d_name);
                    }
                }
            }
            closedir(d);
        }
#endif
    }
    else if (strcmp(cmd, "w") == 0) { 
        if (parsed_args >= 2) {
            strcpy(notes_name, arg1);
        }
        if (notes_mode == 0 && parsed_args >= 2 && strcmp(arg1, "notes.txt") == 0) {
            printf("err: permission denied \n");
        } else {
            get_current_path(loc, notes_name, notes_path);
            file = fopen(notes_path, "w");
            if (file) {
                if (strstr(notes_name, ".seal") != NULL) {
                    printf("ZL SCRIPT EDITOR (%s)\n ", notes_name);
                } else {
                    printf("ZL FILE EDITOR (%s)\nEnter text: ", notes_name);
                }
                fflush(stdout);
                fgets(content, sizeof(content), stdin);
                fprintf(file, "%s", content);
                fclose(file);
                printf("Saved successfully.\n");
            } else {
                printf("err: could not create file\n");
            }
        }
    }
    

else if (strcmp(cmd, "pad") == 0) { 
    if (parsed_args >= 2) {
        strcpy(notes_name, arg1);
    }
    
    if (notes_mode == 0 && parsed_args >= 2 && strcmp(arg1, "code.txt") == 0) {
        printf("err: permission denied \n");
    } else {
        get_current_path(loc, notes_name, notes_path);
        file = fopen(notes_path, "w");
        
        if (file) {
            if (strstr(notes_name, ".seal") != NULL) {
                printf("PAD SCRIPT EDITOR (%s)\n", notes_name);
                printf("Type exit to save and exit\n\n");
            } else {
                printf("PAD FILE EDITOR (%s)\n", notes_name);
                printf("Type exit to save and exit\n\n");
            }

            while (1) {
                fflush(stdout);

                if (fgets(content, sizeof(content), stdin) == NULL) {
                    break; 
                }

                if (strcmp(content, "exit\n") == 0) {
                    break; 
                }

                fprintf(file, "%s", content);
            }

           
            fclose(file);
            printf("saved\n");
        } else {
            printf("err: could not create file\n");
        }
    }
}

    else if (strcmp(cmd, "r") == 0) {
        if (parsed_args < 2) {
            printf("err: missing filename\n");
        } else {
            if (strcmp(arg1, notes_name) == 0 && notes_mode == 0) {
                printf("err: permission denied\n");
            } else {
                char read_path[128], line_buffer[256];
                get_current_path(loc, arg1, read_path);
                FILE *rf = fopen(read_path, "r");
                if (rf) {
                    printf("\n---------------------\n"); 
                    while (fgets(line_buffer, sizeof(line_buffer), rf) != NULL) {
                        printf("%s", line_buffer);
                    }
                    printf("\n---------------------\n"); 
                    fclose(rf);
                } else {
                    printf("err: file locked or missing\n");
                }
            }
        }
    }
    else if (strcmp(cmd, "info") == 0) {
        printf("                ..^~:::::::::::....              \n");
        printf("            .::::.......:^::.. ..:..:::.          \n");
        printf("         ...::..          .:.:.  .::.   .:^:        \n");
        printf("        ::..              .: ~:. ::..      .::      \n");
        printf("      ::.   ..^ :^..      .. . :. ...  :     :^.    \n");
        printf("     .^    P@G:  7&#~      ...         ~       ^.   \n");
        printf("    :^     ~!::!^.:!^  .   ::. ::::.^  ~. .    .^   \n");
        printf("    ^:     ~^?B@@B!:~  ^     .:^..:::...:..     !^. \n");
        printf("   ^:^    ^77GP5PPJ!^  ........ ...  .:   . ^: :^ ~ \n");
        printf("   ^.^:   :...    ^^        ....... :^:^^~:: .^.:^ \n");
        printf("    ..:::...:^:.  ^. .. .. .^.......^^..^:...:^:..  \n");
        printf("        ....:::::^^^~~^^^:^~ : ^:::^^.......       \n");
        printf("                .......^:~^~~^^.               \n");
        printf("\n");
        printf("SealOS 9 \n");
        printf("Code Env.: CodePad\n");
        printf("PC Info: Virtual\n");
        printf("Copyleft SealOS from ZL Project\n");
        printf("2026\n");
    }

    else if (strcmp(cmd, "about") == 0) {
        printf("                ..^~:::::::::::....              \n");
        printf("            .::::.......:^::.. ..:..:::.          \n");
        printf("         ...::..          .:.:.  .::.   .:^:        \n");
        printf("        ::..              .: ~:. ::..      .::      \n");
        printf("      ::.   ..^ :^..      .. . :. ...  :     :^.    \n");
        printf("     .^    P@G:  7&#~      ...         ~       ^.   \n");
        printf("    :^     ~!::!^.:!^  .   ::. ::::.^  ~. .    .^   \n");
        printf("    ^:     ~^?B@@B!:~  ^     .:^..:::...:..     !^. \n");
        printf("   ^:^    ^77GP5PPJ!^  ........ ...  .:   . ^: :^ ~ \n");
        printf("   ^.^:   :...    ^^        ....... :^:^^~:: .^.:^ \n");
        printf("    ..:::...:^:.  ^. .. .. .^.......^^..^:...:^:..  \n");
        printf("        ....:::::^^^~~^^^:^~ : ^:::^^.......       \n");
        printf("                .......^:~^~~^^.               \n");
        printf("\n");
        printf("SealOS 9 \n");
        printf("Code Env.: VM\n");
        printf("Code Env. 2: CodePad\n");
        printf("Code: C, C++\n");
        printf("Host: CodePad Server\n");
        printf("PC Info: Virtual\n");
        printf("Copyleft SealOS from ZL Project\n");
        printf("2026\n");
        printf("QWERTYUIOPASDFGHJKLZXCVBNM1234567890\n");
    }

    else if (strcmp(cmd, "quick") == 0) {
        printf("                ..^~:::::::::::....              \n");
        printf("            .::::.......:^::.. ..:..:::.          \n");
        printf("         ...::..          .:.:.  .::.   .:^:        \n");
        printf("        ::..              .: ~:. ::..      .::      \n");
        printf("      ::.   ..^ :^..      .. . :. ...  :     :^.    \n");
        printf("     .^    P@G:  7&#~      ...         ~       ^.   \n");
        printf("    :^     ~!::!^.:!^  .   ::. ::::.^  ~. .    .^   \n");
        printf("    ^:     ~^?B@@B!:~  ^     .:^..:::...:..     !^. \n");
        printf("   ^:^    ^77GP5PPJ!^  ........ ...  .:   . ^: :^ ~ \n");
        printf("   ^.^:   :...    ^^        ....... :^:^^~:: .^.:^ \n");
        printf("    ..:::...:^:.  ^. .. .. .^.......^^..^:...:^:..  \n");
        printf("        ....:::::^^^~~^^^:^~ : ^:::^^.......       \n");
        printf("                .......^:~^~~^^.               \n");
        printf("\n");
        printf("SealOS 9 \n");
        printf("Code Env.: CodePad\n");
        printf("Code: C, C++\n");
        printf("Host: CodePad Server\n");
        printf("PC Info: Virtual\n");
        printf("Copyleft SealOS from ZL Project\n");
        printf("2026\n");
        printf("QWERTYUIOPASDFGHJKLZXCVBNM1234567890\n");
    }
    else if (strcmp(cmd, "goto") == 0) {
        if (strcmp(arg1, "home") == 0) loc = 1;
        else if (strcmp(arg1, "documents") == 0) loc = 2;
        else if (strcmp(arg1, "downloads") == 0) loc = 3;
        else if (strcmp(arg1, "system") == 0) loc = 4;
        else printf("err: unknown folder\n");
    }
    else if (strcmp(cmd, "back") == 0) {
        loc = 0;
    }
    else if (strcmp(cmd, "where") == 0) {
        const char* locations[] = {"root", "home", "documents", "downloads", "system"};
        if (loc >= 0 && loc <= 4) printf("%s\n", locations[loc]);
    }
    else if (strcmp(cmd, "ls") == 0) {
        printf("home/\ndownloads/\ndocuments/\nsystem/\n");
        FILE *sys_check = fopen("system/sys.txt", "r");
        if (sys_check) {
            printf("  - system/sys.txt\n");
            fclose(sys_check);
        }
        
        if (loc == 3) { 
            FILE *f_html = fopen("downloads/index.html", "r");
            if (f_html) { printf("  └─ downloads/index.html\n"); fclose(f_html); }
            FILE *f_flag = fopen("downloads/flag.txt", "r");
            if (f_flag) { printf("  └─ downloads/flag.txt\n"); fclose(f_flag); }
        } else {
            list_documents_contents();
        }
    }
    else if (strcmp(cmd, "help") == 0) {
        printf("SEALOS SCRIPT LIST:\n");
        printf("goto (Folder) - goes to 1 folder\n");
        printf("ls - list folders and files out\n");
        printf("ls-d - list folders and files out detailed\n");
        printf("info - show OS specification\n");
        printf("about - show OS specification\n");
        printf("quick - show OS specification\n");
        printf("w notes.txt - create/write a file called notes.txt\n");
        printf("r notes.txt - read the contents of notes.txt\n"); 
        printf("rnm notes.txt - rename notes.txt to something else\n");
        printf("chmod notes.txt 1 - change notes.txt to public\n");
        printf("chmod notes.txt 0 - change notes.txt to private\n");
        printf("mkfile (folder name) - create (folder)\n"); 
        printf("where - to show where you are\n");
        printf("back - go back to root\n");
        printf("eggs - secret\n");
        printf("lemon - secret\n");
        printf("echo (text) - repeat (text)\n");
        printf("date - show current date and time\n");
        printf("random - show random numbers\n");
        printf("rm - remove file\n");
        printf("save - download files from websites\n");
        
        printf("w [name].seal - create a executable script\n");
        
        printf("exe [name].seal - run .seal script\n");
    }
    else if (strcmp(cmd, "echo") == 0) {
        printf("%s\n", input + (strlen(input) > 4 ? 5 : 0));
    }
    else if (strcmp(cmd, "date") == 0) {
        time(&currentTime); 
        printf("%s", ctime(&currentTime));
    }
    else if (strcmp(cmd, "random") == 0) {
        printf("%d\n", rand());
    }




















    else if (strcmp(cmd, "lemon") == 0) {
        printf("..................................................\n"
               "...........:::^^~~~~~~~~~^^::.....................\n"
               ".......::^~~~!!!!!!!!!!777777!~^::................\n"
               "...::^~~~~~~~~~~~~!!!!!7777777??77!~:.............\n"
               "..:~~~~~~~~~~~~~~~!!!!7777777777?????!^:..........\n"
               ".:^~~~~~~~~~~~~~!!!!!!!777777777?????J??~:........\n"
               ".:~~~~~~~~~~~~!~!!!!!77777777777???????JJ7^.......\n"
               "..^~~~~~~~!!!!!!!!7777777777?????????????7~......\n"
               "..:~~~~!!!!!!!77777???77???????????????????7^.....\n"
               "..:^~~!!!!!!777777??????????????????????????~.....\n"
               "...:~~~!!!!!7777?7????????JJJ?JJJJ????????JJ?^....\n"
               "....^~~~!!!!77777777???????JJJJJ?????????J????7:..\n"
               ".....:~~!!!!!777777777?????????????????????JJJJ^..\n"
               "......:^~~!!!!!!777777777??????????????????7!!^...\n"
               "........::^~!!!!!7777777777??????????????!:.......\n"
               "...........::^~!!!!777777777???????????!:.........\n");
    }
        
    else if (strcmp(cmd, "eggs") == 0)   printf("                              \n"
        "            .::.              \n"
        "           :!~~~~  ...        \n"
        "          :77!~~7~~!~~~:      \n"
        "        ..^7YJ?JJY7!~!7!      \n"
        "       ^~~^^7JJ?7YYJJJ7^      \n"
        "     .:7?7!~~!~:^!?J?!^.      \n"
        "    ..^!?YYJJ7^..::^:..       \n"
        "     .:^~!77!^:...            \n");
    else if (strcmp(cmd, "exit") == 0)   exit(0);
    else {
        printf("err: command not found\n");
    }
}

int main() {
    srand(time(NULL));
    char input[100];
    FILE *file;

    remove("system/sys.txt");
    remove("documents/notes.txt");
    rmdir("home"); rmdir("downloads"); rmdir("documents"); rmdir("system");

    mkdir("home", 0777);
    mkdir("downloads", 0777);
    mkdir("documents", 0777);
    mkdir("system", 0777);

    file = fopen("system/sys.txt", "w");
    if (file != NULL) {
        fprintf(file, "https://codepad.app/pad/822052z5n");
        fclose(file);
    }
    printf("SealOS 9\n");
    printf("if don't know any command, use 'help'\n");
    while (1) {
        printf("seal> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) break;
        process_system_command(input);
    }
    return 0;
}
