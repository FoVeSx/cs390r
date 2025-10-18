#include <stdio.h>
#include <stdlib.h>
#include <string.h>

__attribute__((constructor)) void ignore_me(){
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
}

int registered = 0;
long long seed;

typedef struct Service {
    char *name;
    char *url;
    long long pw;
} Service;

typedef struct User {
    char *username;
    long long master_password;
    int service_count;
    Service *services[10];
} User;

int error_exit(char *err) {
    printf("%s", err);
    exit(1);
}

long long get_pw(long long input) {
    long long ret = seed ^ input;
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 43;
    return ret;
}

int add_service(User *user) {
    char *m = NULL;
    char buf[0x8];
    int choice;
    long long pw;
    Service *service = (Service*)malloc(sizeof(Service));

    if (user->service_count >= 10) {
        puts("Already registered maximum amount of services available for free version");
        return 1;
    }

    puts("Enter service name");
    if (scanf("%ms", &m) != 1) { error_exit("Failed to read in service-name"); };
    service->name = malloc(strlen(m));
    strcpy(service->name, m);
    m = NULL;

    puts("Enter service url");
    if (scanf("%ms", &m) != 1) { error_exit("Failed to read in service-name"); };
    service->url = malloc(strlen(m));
    strcpy(service->url, m);

    puts("Do you want your password to be automatically generated (this is generally safer)?");
    getchar();
    if (fgets(buf, sizeof(buf), stdin) == NULL) { error_exit("Failed to read in choice"); }
    choice = atoi(buf);

    printf("choice: %d", choice);

    // Letting users chose their own passwords just asks them to chose '1234'. This is not safe.
    if (choice == 0) {
        puts("What do you want as your password?");
        if (scanf("%lld", &pw) != 1) { error_exit("Failed to read in service-password "); };
        service->pw = pw;
    } else {
        service->pw = get_pw(user->master_password);
    }
    user->services[user->service_count] = service;
    user->service_count++;

    puts("Service successfully added");

    return 0;
}

int view_service(User *user) {
    char *m = NULL;

    puts("Which service do you want to view");
    if (scanf("%ms", &m) != 1) { error_exit("Failed to read in service-name"); };

    for (int i = 0; i < user->service_count; i++) {
        if (strcmp(user->services[i]->name, m) == 0) {
            printf("name: %s\n", user->services[i]->name);
            printf("url: %s\n", user->services[i]->url);
            printf("password: %lld\n", user->services[i]->pw);
            return 0;
        }
    }

    puts("Service name does not exist");
    return 1;
}

int delete_service(User *user) {
    char buf[0x8];
    int index;

    puts("Which service do you want to view");
    if (fgets(buf, sizeof(buf), stdin) == NULL) { error_exit("Failed to read in choice"); }
    index = atoi(buf);

    if (index < user->service_count) {
        free(user->services[index]->name);
        free(user->services[index]->url);
        free(user->services[index]);
        puts("Service successfully deleted");
    } else {
        printf("You cannot delete a service that doesn't exist\n");
        return 1;
    }

    return 0;
}

int register_user(User *user) {
    char *m = NULL;
    long long master1, master2;

    puts("Username?");

    if (scanf("%ms", &m) != 1) { error_exit("Failed to read in username"); };
    user->username = malloc(strlen(m));
    strcpy(user->username, m);

    puts("Set your master password");
    if (scanf("%lld", &master1) != 1) { error_exit("Failed to read in master_password1 "); };

    puts("Confirm your master password");
    if (scanf("%lld", &master2) != 1) { error_exit("Failed to read in master_password2 "); };

    if (master1 != master2) {
        error_exit("master passwords did not match");
    } else {
        user->master_password = master1;
        user->service_count = 0;
        printf("Successfuly registered account: %s\n", user->username);
    }

    free(m);
    return 0;
}

int pw_manager(User *user) {
    int choice;
    char buf[0x8];

    while (1) {
        puts("\n+====:[ Super-Secure Password Manager ]:====+");
        puts("| [1] Add Service    |");
        puts("| [2] View Service   |");
        puts("| [3] Delete Service |");
        puts("| [4] Logout         |");
        puts("+===========================+");
        printf("\n > ");

        if (fgets(buf, sizeof(buf), stdin) == NULL) { error_exit("Failed to read in choice"); }
        choice = atoi(buf);

        switch (choice) {
            case 1:
                add_service(user);
                break;
            case 2:
                view_service(user);
                break;
            case 3:
                delete_service(user);
            default:
                puts("Not a valid option");
        }
    }
}

int login(User *user) {
    char buf[0x8];
    long long pw;

    puts("Enter password");
    if (fgets(buf, sizeof(buf), stdin) == NULL) { exit(1); }
    pw = atoi(buf);

    if (user->master_password == pw) {
        pw_manager(user);
        return 0;
    } else {
        puts("Password Incorrect");
        return 1;
    }
}

int main() {
    User *user = NULL;
    char buf[8];
    int choice;

    // We can use exploit mitigations like aslr to get a random address as our seed
    seed = (long long) &puts;

    while (1) {
        puts("Do you wish to Register [0] a new account or Login [1] with an existing account?");
        if (fgets(buf, sizeof(buf), stdin) == NULL) { exit(1); }
        choice = atoi(buf);

        if (choice == 0) {
            if (registered > 0) {
                printf("Already registered your account");
            } else {
                user = (User*)malloc(sizeof(User));
                register_user(user);
                registered = 1;
            }
        } else if (choice == 1) {
            if (user == NULL) {
                puts("No user registered yet");
            } else {
                login(user);
            }
        } else {
            puts("Not a valid option");
        }
    }
    return 0;
}
