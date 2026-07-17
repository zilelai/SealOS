//COPYLEFT FROM ZILELAI/ZL PROJECTS 2026
//THIS FILE IS LICENSED BY GNU 3.0 LICENSE IN GITHUB
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <float.h>
#include <sys/stat.h>
#include <unistd.h>
#include "math.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;

    char *ptr = (char *)realloc(*(char **)userp, strlen(*(char **)userp) + realsize + 1);
    if(ptr == NULL) return 0; 
    
    *(char **)userp = ptr;
    
    strncat(*(char **)userp, (const char *)contents, realsize);
    return realsize;
}

char op;
double firstnum;
double secondnum;
double result;
char cmd[50];
char input_buf[50];
int loc = 0;
int notes_mode = 0;
char notes_name[64] = "notes.txt";
char notes_path[128];
int q = 1;
char w = 'A';
float e = 5.7293;
double r = 4.882823923923923823283;
long t = 832832832732;
int superior;






void get_current_path(int current_loc, const char* filename, char* out_path) {
    switch(current_loc) {
        case 1: sprintf(out_path, "home/%s", filename); break;
        case 2: sprintf(out_path, "documents/%s", filename); break;
        case 3: sprintf(out_path, "downloads/%s", filename); break;
        case 4: sprintf(out_path, "system/%s", filename); break;
        case 5: sprintf(out_path, "music/%s", filename); break;
        case 6: sprintf(out_path, "pictures/%s", filename); break;
        case 7: sprintf(out_path, "videos/%s", filename); break;
        case 8: sprintf(out_path, "examples/%s", filename); break;
        default: sprintf(out_path, "%s", filename); break; 
    }
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

    else if (strcmp(cmd, "mv") == 0) {
        char o[128];
        char n[128];
        printf("Key in your old file location: ");
        scanf("%127s", o); 
        printf("Key in your new file location: ");
        scanf("%127s", n);
    
        if (rename(o, n) == 0) {
            printf("File moved successfully.\n");
        } else {
           perror("Error"); 
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
        if (parsed_args >= 2) {
            strcpy(notes_name, arg1);
        } else {
            printf("err: missing filename.\n");
            return; 
        }
        
        if (notes_mode == 0 && strcmp(arg1, "code.txt") == 0) {
            printf("err: permission denied \n");
        } else {
            char compile[512];
            snprintf(compile, sizeof(compile), "gcc %s -o main && ./main", arg1);
            
            system(compile);
            printf("\n");
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
    else if (strcmp(cmd, "ls-t") == 0) {
        DIR *dir;
        struct dirent *entry;
        char fpath[1024];
    
        dir = opendir(".");
        if (dir == NULL){
            printf("err: file locked or missing\n");
            return;
        }

        while ((entry = readdir(dir)) != NULL){
            if (entry->d_type == DT_REG){
                printf("-: %s\n", entry->d_name);
            }
            else if (entry->d_type == DT_DIR){
                printf("d: %s\n", entry->d_name);
            }
           
            
        }

        if (closedir(dir) == -1){
            printf("err: file locked or missing\n");
            return;
        }
    }
    else if (strcmp(cmd, "ls-d") == 0) {
        DIR *dir;
        struct dirent *entry;
        char fpath[1024];
        char read;
        char write;
        char execute;
        char null;
    
        dir = opendir(".");
        if (dir == NULL){
            printf("err: file locked or missing\n");
            return;
        }

        while ((entry = readdir(dir)) != NULL){
            snprintf(fpath, sizeof(fpath), "./%s", entry->d_name);

            struct stat pstd;
            if (stat(fpath, &pstd) != 0) {
                perror("err: file status unavailable");
                continue;
            }

            if (S_ISREG(pstd.st_mode)){
                int r = (access(fpath, R_OK) == 0);int w = (access(fpath, W_OK) == 0);int x = (access(fpath, X_OK) == 0);

                if (r == 1){read = 'r';}
                if (w == 1){write = 'w';}
                if (x == 1){execute = 'x';}
                if (r == 0){read = '-';}
                if (w == 0){write = '-';}
                if (x == 0){execute = '-';}
                printf("%c/%c/%c/%s\n", read,write,execute, entry->d_name);
                
            }
        }
        if (closedir(dir) == -1){
            printf("err: file locked or missing\n");
            return;
        }
    }

    else if (strcmp(cmd, "ls-dt") == 0) {
        DIR *dir;
        struct dirent *entry;
        char fpath[1024];
        char read;
        char write;
        char execute;
        char null;
    
        dir = opendir(".");
        if (dir == NULL){
            printf("err: file locked or missing\n");
            return;
        }
    
        while ((entry = readdir(dir)) != NULL) {
            snprintf(fpath, sizeof(fpath), "%s", entry->d_name); 
        
            struct stat pstd;
            
           
            if (stat(fpath, &pstd) != 0) {
                continue; 
            }
        
            int r = (access(fpath, R_OK) == 0);
            int w = (access(fpath, W_OK) == 0);
            int x = (access(fpath, X_OK) == 0);
        
            if (r == 1){read = 'r';}
            if (w == 1){write = 'w';}
            if (x == 1){execute = 'x';}
            if (r == 0){read = '-';}
            if (w == 0){write = '-';}
            if (x == 0){execute = '-';}
            
            if (S_ISREG(pstd.st_mode)) {
                printf("-/%c/%c/%c/%s\n", read, write, execute, entry->d_name);
            } 
            else if (S_ISDIR(pstd.st_mode)) {
                printf("d/%c/%c/%c/%s\n", read, write, execute, entry->d_name);
            }
        }
    
        if (closedir(dir) == -1){
            printf("err: file locked or missing\n");
            return;
        }
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
            printf("PAD EDITOR (%s)\n", notes_name);
            printf("Type exit to save and exit\n\n");
            char line_buffer[512]; 
            get_current_path(loc, notes_name, notes_path);
            file = fopen(notes_path, "a+");
            
            if (file) {
                fseek(file, 0, SEEK_SET); 
                while (fgets(line_buffer, sizeof(line_buffer), file) != NULL) {
                    printf("%s", line_buffer);
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
                printf("err: could not open file\n");
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

        printf("                 ..^~:::::::::::....              \n");
            printf("            .::::.......:^::.. ..:..:::.          \n");
            printf("       ...::..          .:.:.  .::.   .:^:        \n");
            printf("      ::..              .: ~:. ::..      .::      \n");
            printf("    ::.   ..^ :^..      .. . :. ...  :     :^.    \n");
            printf("   .^    P@G:  7&#~      ...         ~       ^.   \n");
            printf("  :^     ~!::!^.:!^  .   ::. ::::.^  ~. .    .^   \n");
            printf("  ^:     ~^?B@@B!:~  ^     .:^..:::...:..     !^. \n");
            printf(" ^:^    ^77GP5PPJ!^  ........ ...  .:   . ^: :^ ~ \n");
            printf(" ^.^:   :...    ^^         ....... :^:^^~:: .^.:^ \n");
            printf("  ..:::...:^:.  ^. .. .. .^.......^^..^:...:^:..  \n");
            printf("       ....:::::^^^~~^^^:^~ : ^:::^^.......       \n");
            printf("                   .......^:~^~~^^.               \n");
            printf("\n");
        printf("SealKernel 16.7.2026 EDU\n");
        printf("Code Env.: CodePad\n");
        printf("PC Info: Virtual\n");
        printf("Copyleft SealKernel from ZL Project\n"); 
        printf("2026\n");
        
        }
        
        
        else if (strcmp(cmd, "about") == 0) {
            printf("                 ..^~:::::::::::....              \n");
            printf("            .::::.......:^::.. ..:..:::.          \n");
            printf("       ...::..          .:.:.  .::.   .:^:        \n");
            printf("      ::..              .: ~:. ::..      .::      \n");
            printf("    ::.   ..^ :^..      .. . :. ...  :     :^.    \n");
            printf("   .^    P@G:  7&#~      ...         ~       ^.   \n");
            printf("  :^     ~!::!^.:!^  .   ::. ::::.^  ~. .    .^   \n");
            printf("  ^:     ~^?B@@B!:~  ^     .:^..:::...:..     !^. \n");
            printf(" ^:^    ^77GP5PPJ!^  ........ ...  .:   . ^: :^ ~ \n");
            printf(" ^.^:   :...    ^^         ....... :^:^^~:: .^.:^ \n");
            printf("  ..:::...:^:.  ^. .. .. .^.......^^..^:...:^:..  \n");
            printf("       ....:::::^^^~~^^^:^~ : ^:::^^.......       \n");
            printf("                   .......^:~^~~^^.               \n");
            printf("\n");
        
            FILE *fp = fopen("main.cpp", "rb");

        if (fp == NULL){
            perror("err: file unopened");
            
        }

        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        

        fclose(fp);

        printf("SealKernel 16.7.2026 EDU\n");
        
        printf("Code Env.: VM\n");
        
        printf("Code Env. 2: CodePad\n");
        
        printf("Code: C, C++\n");
        
        printf("Host: CodePad Server\n");
        
        printf("PC Info: Virtual\n");

        printf("SealKernel Size: %ld bytes\n", size);
        
        printf("Copyleft SealKernel from ZL Project\n");
        
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

    else if (strcmp(cmd, "sudo-on") == 0){
        superior = 1;
        printf("sudo mode\n");
    }
    else if (strcmp(cmd, "sudo-off") == 0){
        superior = 0;
        printf("normal mode\n");
    }
    else if (strcmp(cmd, "back") == 0) {
        loc = 0;
    }
    else if (strcmp(cmd, "where") == 0) {
        const char* locations[] = {"root", "home", "documents", "downloads", "system"};

        if (loc >= 0 && loc <= 4) printf("%s\n", locations[loc]);
        else if (superior == 1) printf("superior/%s\n", locations[loc]);
    }
    else if (strcmp(cmd, "version") == 0) {
        printf("SealKernel 16.7.2026 EDU\n");
    }
    else if (strcmp(cmd, "release") == 0){
        printf("SealKernel 16.7.2026 EDU - branched out from SealKernel 16.7.2026. This is strictly for educational use\n");}
    else if (strcmp(cmd, "ls") == 0) {
        DIR *dir;
        struct dirent *entry;
    
        dir = opendir(".");
        if (dir == NULL){
            printf("err: file locked or missing\n");
            return;
        }

        while ((entry = readdir(dir)) != NULL){
            printf("%s", entry->d_name);
            printf("\n");
            
        }

        if (closedir(dir) == -1){
            printf("err: file locked or missing\n");
            return;
        }
            
    }
    else if (strcmp(cmd, "help") == 0) {
        printf("SEALOS SCRIPT LIST:\n");
        printf("goto (Folder) - goes to 1 folder\n");
        printf("ls - list folders and files out\n");
        printf("ls-d - list folders and files out detailed\n");
        printf("info - show OS specification\n");
        printf("about - show OS specification\n");
        printf("w notes.txt - create/write a file called notes.txt\n");
        printf("r notes.txt - read the contents of notes.txt\n"); 
        printf("rnm notes.txt - rename notes.txt to something else\n");
        printf("chmod notes.txt 1 - change notes.txt to public\n");
        printf("chmod notes.txt 0 - change notes.txt to private\n");
        printf("mkfile (folder name) - create (folder)\n"); 
        printf("where - to show where you are\n");
        printf("back - go back to root\n");
        printf("echo (text) - repeat what you had key in\n");
        printf("date - show current date and time\n");
        printf("random - show random numbers\n");
        printf("rm - remove file\n");
        printf("save - download files from websites\n");
        printf("w [name].seal - create a executable script\n");
        printf("exe [name].seal - run .seal script\n");
        printf("release - check what has updated\n");
        printf("version - check only the version\n");
        printf("curl - search something online (might not work in online compiler)\n");
        printf("tsastream - check your avg score for TSIS students\n");
        printf("calc - simple calculator that you have to key in manually\n");
        printf("space - to check space in main.cpp\n");
        printf("space-downloads - to check space in downloads\n");
        printf("space-documents - to check space in documents\n");
        printf("space-home - to check space in home\n");
        printf("space-music - to check space in music\n");
        printf("space-pictures - to check space in pictures\n");
        printf("space-videos - to check space in videos\n");
        printf("space-examples - to check space in examples\n");
        printf("space-system - to check space in system\n");
        printf("space-others - to check space in other file/directory(folder)\n");
        printf("sudo-on - change to sudo user\n");
        printf("sudo-off - change to normal user \n");
        printf("sudo-exit - shut down only with sudo permissions\n");
        printf("ls-t - check file type\n");
        printf("ls-dt - check file type and in advance\n");
        printf("mv - move file\n");
        printf("image - display image\n");
    }
    else if (strcmp(cmd, "echo") == 0) {
        printf("%s\n", input + (strlen(input) > 4 ? 5 : 0));
    }
    else if (strcmp(cmd, "tsastream") == 0) {
        
        
        float math;
        float chemistry;
        float physics;
        float biology;
        float history;
        float geography;
        
        
        printf("Input your math marks: ");
        scanf("%f", &math);
        printf("Input your chemistry marks: ");
        scanf("%f", &chemistry);
        printf("Input your biology marks: ");
        scanf("%f", &biology);
        printf("Input your physics marks: ");
        scanf("%f", &physics);
        printf("Input your geography marks: ");
        scanf("%f", &geography);
        printf("Input your history marks: ");
        scanf("%f", &history);


        float math_pct = math * 0.55;
        float chem_pct = chemistry * 0.125;
        float bio_pct = biology * 0.1;
        float phy_pct = physics * 0.125;
        float geo_pct = geography * 0.05;
        float his_pct = history * 0.05;

        int avg = math_pct+chem_pct+bio_pct+phy_pct+geo_pct+his_pct;

        
        printf("Your streaming mark is: %.d%%\n", avg);
        if (avg >= 90){
            printf("2025 streaming mark: Acacia\n");
        }
        else if (avg >= 80 && avg < 90){
            printf("2025 streaming mark: Aster\n");
        }

        else if (avg >= 70 && avg < 80){
            printf("2025 streaming mark: Begonia\n");
        }

        else if (avg >= 60 && avg < 70){
            printf("2025 streaming mark: Castanea\n");
        }

        else if (avg >= 50 && avg < 60){
            printf("2025 streaming mark: Juniper / MX Intensive 1\n");
        }

        else if (avg < 50){
            printf("2025 streaming mark: Magnolia/ MX Intensive 2 / MX Intensive\n");
        }

        if (avg >= 70){
            printf("2026 streaming mark: Acacia\n");
        }
        else if (avg >= 60 && avg < 70){
            printf("2026 streaming mark: Aster\n");
        }

        else if (avg >= 50 && avg < 60){
            printf("2026 streaming mark: Begonia\n");
        }

        else if (avg >= 40 && avg < 50){
            printf("2026 streaming mark: Castanea\n");
        }

        else if (avg < 40){
            printf("2026 streaming mark: MX Intensive 1 & 2\n");
        }
        

        printf("Now writing into a text file\n");

        FILE *fptr;

        fptr = fopen("grades.txt", "w");
        fprintf(fptr, "Math,");
        fprintf(fptr, "%f", math_pct);
        fprintf(fptr, "\nPhysics,");
        fprintf(fptr,"%f", phy_pct);
        fprintf(fptr, "\nChemistry,");
        fprintf(fptr,"%f", chem_pct);
        fprintf(fptr,"\nBiology,");
        fprintf(fptr,"%f", bio_pct);
        fprintf(fptr, "\nHistory,");
        fprintf(fptr,"%f", his_pct);
        fprintf(fptr, "\nGeography,");
        fprintf(fptr,"%f", geo_pct);
        fprintf(fptr, "\nAverage,");
        fprintf(fptr,"%d", avg);
        fprintf(fptr, "\n");
        fclose(fptr);
        printf("Writing Complete :)\n");


        
    }
    else if (strcmp(cmd, "date") == 0) {
        time(&currentTime); 
        printf("%s", ctime(&currentTime));
    }
    
    else if (strcmp(cmd, "random") == 0) {
        printf("%d\n", rand());
    }
    
    else if (strcmp(cmd, "calc") == 0) {
        double output;
        printf("Before procedding, if u want to calculate roots, the big number is the first number and the root is the second number. Same applies to powers.\n");
        printf("Enter your case: (+, -, *, /, 'R' for roots, ^ for powers)\n");
        scanf("%c", &op);
        printf("Enter your first number:\n");
        scanf("%lf", &firstnum);
        printf("Enter your second number:\n");
        scanf("%lf", &secondnum);

        if (op == '+'){
            double output = firstnum + secondnum;
            printf("%lf\n", output);
        }
        else if (op == '-'){
            double output = firstnum - secondnum;
            printf("%lf\n", output);
        }
        else if (op == '*'){
            double output = firstnum * secondnum;
            printf("%lf\n", output);
        }
        else if (op == '/'){
            double output = firstnum / secondnum;
            printf("%lf\n", output);
        }
        else if (op == '^'){
            double output = pow(firstnum, secondnum);
            printf("%lf\n", output);
        }
        else if (op == 'R'){
            double output = pow(firstnum, 1.0 / secondnum);
            printf("%lf\n", output);
        }
        else{
            printf("err: operator not found");
        }
    }
    

    else if (strcmp(cmd, "space") == 0){
        FILE *fp = fopen("main.cpp", "rb");
        

        if (fp == NULL){
            perror("err: file unopened");
            
        }

        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        printf("main.cpp: %ld bytes\n", size);

        fclose(fp);
    }

    else if (strcmp(cmd, "space-documents") == 0){
        FILE *fp = fopen("documents", "rb");
        

        if (fp == NULL){
            perror("err: file unopened");
            
        }

        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        printf("documents: %ld bytes\n", size);

        fclose(fp);
    }

    else if (strcmp(cmd, "space-downloads") == 0){
        FILE *fp = fopen("downloads", "rb");
        

        if (fp == NULL){
            perror("err: file unopened");
            
        }

        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        printf("downloads: %ld bytes\n", size);

        fclose(fp);
    }

    else if (strcmp(cmd, "space-home") == 0){
        FILE *fp = fopen("home", "rb");
        

        if (fp == NULL){
            perror("err: file unopened");
            
        }

        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        printf("home: %ld bytes\n", size);

        fclose(fp);
    }
    else if (strcmp(cmd, "space-examples") == 0){
        FILE *fp = fopen("examples", "rb");
        

        if (fp == NULL){
            perror("err: file unopened");
            
        }

        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        printf("examples: %ld bytes\n", size);

        fclose(fp);
    }
    else if (strcmp(cmd, "space-videos") == 0){
        FILE *fp = fopen("videos", "rb");
        

        if (fp == NULL){
            perror("err: file unopened");
            
        }

        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        printf("videos: %ld bytes\n", size);

        fclose(fp);
    }
    else if (strcmp(cmd, "space-pictures") == 0){
        FILE *fp = fopen("pictures", "rb");
        

        if (fp == NULL){
            perror("err: file unopened");
            
        }

        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        printf("pictures: %ld bytes\n", size);

        fclose(fp);
    }
    else if (strcmp(cmd, "space-music") == 0){
        FILE *fp = fopen("home", "rb");
        

        if (fp == NULL){
            perror("err: file unopened");
            
        }

        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        printf("music: %ld bytes\n", size);

        fclose(fp);
    }
    else if (strcmp(cmd, "space-system") == 0){
        FILE *fp = fopen("system", "rb");
        

        if (fp == NULL){
            perror("err: file unopened");
            
        }

        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        printf("system: %ld bytes\n", size);

        fclose(fp);
    }

    else if (strcmp(cmd, "space-others") == 0){
        char input [256];
        printf("What file or folder(directory)? ");
        scanf("%255s", input);
        FILE *fp = fopen(input, "rb");
        

        if (fp == NULL){
            perror("err: file unopened");
            return;
            
        }

        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        printf("%s", input);
        printf(": %ld bytes\n", size);

        fclose(fp);
    }


    else if (strcmp(cmd, "image") == 0) { 
        char chars[] = "`^\",:;Il!i~+_-?][}(1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao#MW&8%B@S";
        int charslen = strlen(chars);

        char input[128]; 
        int width, height, pixel;
    
        printf("Type what file do you want to show in ASCII?\n");
    
        if (scanf("%127s", input) != 1) {
            printf("err: input failed to read\n");
            return; 
        }
        

        unsigned char *ImageData = stbi_load(input, &width, &height, &pixel, 0);
    
        if (ImageData) {
            int stepX = 4;  
            int stepY = 8;  
    
            for (int rownum = 0; rownum < height; rownum += stepY) 
            {
                for (int colnum = 0; colnum < width; colnum += stepX) 
                {
                    int pixelOffset = (rownum * width + colnum) * pixel;
                    unsigned char *pixelpointer = ImageData + pixelOffset;
                    
                    unsigned char R = pixelpointer[0];
                    unsigned char G = pixelpointer[1];
                    unsigned char B = pixelpointer[2];
                    
                    float Avg = (R + G + B) / 3.0f; 
                    int charnum = (int)(charslen * (Avg / 255.0f));
                    if (charnum >= charslen) charnum = charslen - 1;
                    
                    putchar(chars[charnum]);
                    putchar(chars[charnum]); 
                }
                putchar('\n');
            }
            stbi_image_free(ImageData);
    
        } else {
            printf("err: image loading failed, %s\n", stbi_failure_reason());
        }
    }
    
    else if (strcmp(cmd, "sudo-exit") == 0) {
        if (superior == 1){exit(0);}
        else {printf("err: sudo user undetected, switch to sudo user\n");}
        
    }

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
    printf("SealKernel 16.7.2026 EDU\n");
    printf("A project by ZileLai\n");
    printf("if don't know any command, use 'help'\n");

    while (1) {
        
        if (loc == 1 && superior == 0){
            printf("seal/home> ");
        }
        else if (loc == 2 && superior == 0){
            printf("seal/documents> ");
        }
        else if (loc == 3 && superior == 0){
            printf("seal/downloads> ");
        }
        else if (loc == 4 && superior == 0){
            printf("seal/system> ");
        }
        else if (loc == 0 && superior == 0){
            printf("seal> ");
        }
        else if (loc == 1 && superior == 1){
            printf("superior/home> ");
        }
        else if (loc == 2 && superior == 1){
            printf("superior/documents> ");
        }
        else if (loc == 3 && superior == 1){
            printf("superior/downloads> ");
        }
        else if (loc == 4 && superior == 1){
            printf("superior/system> ");
        }
        else if (loc == 0 && superior == 1){
            printf("superior/seal> ");
        }
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) break;
        
        if (strcmp(input, "curl\n") == 0) {

            #ifdef LIBCURL_AVAILABLE
                printf("running curl...\n");
                break;
            #else
                printf("err: curl not supported here\n");
                continue;
            #endif
        }
        process_system_command(input);
    }


    #ifdef LIBCURL_AVAILABLE
        CURL *curl;
        CURLcode res;
        char *response_string = malloc(1); 
        if (response_string == NULL) return 1;
        response_string[0] = '\0';

        curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init(); 

        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, "https://google.com");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

            res = curl_easy_perform(curl);

            if(res != CURLE_OK) {
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            } else {
                printf("%s\n", response_string);
            }

            curl_easy_cleanup(curl);
        }
        
        free(response_string);
        curl_global_cleanup();
    #endif

    return 0;
}
