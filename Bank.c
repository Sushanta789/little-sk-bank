#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define DATA_FILE "bankdata.txt"
#define MAX_ACCOUNTS 1000
#define MAX_TRANSACTIONS 100

typedef struct {
    char datetime[20];
    char type[10]; // Deposit or Withdraw
    double amount;
} Transaction;

typedef struct {
    char firstName[30];
    char lastName[30];
    char gender[10];
    char email[50];
    char mobile[15];
    char accountNumber[15];  // SK + 10 digits
    char customerID[15];     // CUST + 6 digits
    char pin[5];             // 4 digit pin + \0
    char ifcCode[10];        // IFC + 6 digits
    char atmCard[20];        // ATM + 16 digits
    double balance;
    Transaction transactions[MAX_TRANSACTIONS];
    int transactionCount;
} Account;

Account accounts[MAX_ACCOUNTS];
int accountCount = 0;

void sleep_ms(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

void animateText(const char *text) {
    for (int i = 0; text[i]; i++) {
        putchar(text[i]);
        fflush(stdout);
        sleep_ms(30);
    }
    putchar('\n');
}

void generateRandomNumber(char *prefix, int totalLength, char *output) {
    int prefixLen = strlen(prefix);
    strcpy(output, prefix);
    for (int i = prefixLen; i < totalLength; i++) {
        output[i] = '0' + rand() % 10;
    }
    output[totalLength] = '\0';
}

void getCurrentDateTime(char *buffer) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d",
        tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
        tm->tm_hour, tm->tm_min, tm->tm_sec);
}

void saveData() {
    FILE *fp = fopen(DATA_FILE, "w");
    if (!fp) {
        printf("Error saving data.\n");
        return;
    }
    fprintf(fp, "%d\n", accountCount);
    for (int i = 0; i < accountCount; i++) {
        Account *acc = &accounts[i];
        fprintf(fp, "%s|%s|%s|%s|%s|%s|%s|%s|%s|%.2f|%d\n",
                acc->firstName, acc->lastName, acc->gender, acc->email,
                acc->mobile, acc->accountNumber, acc->customerID,
                acc->pin, acc->ifcCode, acc->balance, acc->transactionCount);
        for (int j = 0; j < acc->transactionCount; j++) {
            fprintf(fp, "%s|%s|%.2f\n", acc->transactions[j].datetime,
                    acc->transactions[j].type, acc->transactions[j].amount);
        }
    }
    fclose(fp);
}

void loadData() {
    FILE *fp = fopen(DATA_FILE, "r");
    if (!fp) {
        accountCount = 0;
        return; // No data yet
    }
    fscanf(fp, "%d\n", &accountCount);
    for (int i = 0; i < accountCount; i++) {
        Account *acc = &accounts[i];
        fscanf(fp, "%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%lf|%d\n",
               acc->firstName, acc->lastName, acc->gender, acc->email,
               acc->mobile, acc->accountNumber, acc->customerID,
               acc->pin, acc->ifcCode, &acc->balance, &acc->transactionCount);
        for (int j = 0; j < acc->transactionCount; j++) {
            fscanf(fp, "%[^|]|%[^|]|%lf\n", acc->transactions[j].datetime,
                   acc->transactions[j].type, &acc->transactions[j].amount);
        }
    }
    fclose(fp);
}

void otpVerification() {
    animateText("Sending OTP to your mobile...");
    sleep_ms(1000);
    animateText("OTP verified successfully!");
}

void registerAccount() {
    if (accountCount >= MAX_ACCOUNTS) {
        animateText("Maximum accounts reached!");
        return;
    }

    Account *acc = &accounts[accountCount];
    char input[100];

    animateText("=== Registration ===");

    printf("Enter First Name: ");
    fgets(acc->firstName, sizeof(acc->firstName), stdin);
    strtok(acc->firstName, "\n");

    printf("Enter Last Name: ");
    fgets(acc->lastName, sizeof(acc->lastName), stdin);
    strtok(acc->lastName, "\n");

    printf("Select Gender (Male/Female): ");
    fgets(acc->gender, sizeof(acc->gender), stdin);
    strtok(acc->gender, "\n");

    printf("Enter Email ID: ");
    fgets(acc->email, sizeof(acc->email), stdin);
    strtok(acc->email, "\n");

    printf("Enter Mobile Number: ");
    fgets(acc->mobile, sizeof(acc->mobile), stdin);
    strtok(acc->mobile, "\n");

    otpVerification();

    // Set 4-digit PIN
    while (1) {
        printf("Set 4-digit PIN: ");
        fgets(input, sizeof(input), stdin);
        strtok(input, "\n");
        if (strlen(input) == 4 && strspn(input, "0123456789") == 4) {
            strcpy(acc->pin, input);
            break;
        } else {
            animateText("Invalid PIN. Enter exactly 4 digits.");
        }
    }

    // Generate account details
    generateRandomNumber("SK", 12, acc->accountNumber);
    generateRandomNumber("CUST", 10, acc->customerID);
    generateRandomNumber("IFC", 9, acc->ifcCode);
    generateRandomNumber("ATM", 16, acc->atmCard);
    acc->balance = 0.0;
    acc->transactionCount = 0;

    accountCount++;
    saveData();
    animateText("Registration successful!");
    printf("Your Account Number: %s\n", acc->accountNumber);
    printf("Your Customer ID: %s\n", acc->customerID);
}

Account* loginAccount() {
    char accNum[20], pin[10];
    animateText("=== Login ===");
    printf("Enter Account Number: ");
    fgets(accNum, sizeof(accNum), stdin);
    strtok(accNum, "\n");

    int found = -1;
    for (int i = 0; i < accountCount; i++) {
        if (strcmp(accounts[i].accountNumber, accNum) == 0) {
            found = i;
            break;
        }
    }
    if (found == -1) {
        animateText("Account not found!");
        return NULL;
    }

    int attempts = 3;
    while (attempts > 0) {
        printf("Enter 4-digit PIN: ");
        fgets(pin, sizeof(pin), stdin);
        strtok(pin, "\n");

        if (strcmp(accounts[found].pin, pin) == 0) {
            animateText("Login successful!");
            printf("Welcome %s %s\n", accounts[found].firstName, accounts[found].lastName);
            return &accounts[found];
        } else {
            attempts--;
            printf("Incorrect PIN. Attempts left: %d\n", attempts);
        }
    }
    animateText("Too many failed attempts. Returning to main menu.");
    return NULL;
}

void showTransactions(Account *acc) {
    if (acc->transactionCount == 0) {
        animateText("No transactions found.");
        return;
    }
    animateText("Transaction History:");
    for (int i = 0; i < acc->transactionCount; i++) {
        printf("%s | %s | ₹%.2lf\n", acc->transactions[i].datetime, acc->transactions[i].type, acc->transactions[i].amount);
    }
}

void depositMoney(Account *acc) {
    char input[50];
    double amount = 0;
    printf("Enter amount to deposit: ₹");
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%lf", &amount);
    if (amount <= 0) {
        animateText("Invalid amount.");
        return;
    }
    acc->balance += amount;

    // Record transaction
    if (acc->transactionCount < MAX_TRANSACTIONS) {
        char dt[20];
        getCurrentDateTime(dt);
        strcpy(acc->transactions[acc->transactionCount].datetime, dt);
        strcpy(acc->transactions[acc->transactionCount].type, "Deposit");
        acc->transactions[acc->transactionCount].amount = amount;
        acc->transactionCount++;
    }

    animateText("Deposit successful!");
    printf("New balance: ₹%.2lf\n", acc->balance);
}

void withdrawMoney(Account *acc) {
    char input[50];
    double amount = 0;
    printf("Enter amount to withdraw: ₹");
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%lf", &amount);
    if (amount <= 0) {
        animateText("Invalid amount.");
        return;
    }
    if (amount > acc->balance) {
        animateText("Insufficient balance!");
        return;
    }
    acc->balance -= amount;

    // Record transaction
    if (acc->transactionCount < MAX_TRANSACTIONS) {
        char dt[20];
        getCurrentDateTime(dt);
        strcpy(acc->transactions[acc->transactionCount].datetime, dt);
        strcpy(acc->transactions[acc->transactionCount].type, "Withdraw");
        acc->transactions[acc->transactionCount].amount = amount;
        acc->transactionCount++;
    }

    animateText("Withdrawal successful!");
    printf("New balance: ₹%.2lf\n", acc->balance);
}

void showAtmCard(Account *acc) {
    animateText("=== ATM Card Details ===");
    printf("Account Holder: %s %s\n", acc->firstName, acc->lastName);
    printf("ATM Card Number: %s\n", acc->atmCard);
    printf("Account Number: %s\n", acc->accountNumber);
    printf("IFC Code: %s\n", acc->ifcCode);
}

void accountMenu(Account *acc) {
    while (1) {
        printf("\n1. Check Balance\n2. Deposit Money\n3. Withdraw Money\n4. Show ATM Card\n5. Show Transaction History\n6. Logout\nEnter choice: ");
        char choice[10];
        fgets(choice, sizeof(choice), stdin);

        switch (choice[0]) {
            case '1':
                printf("Current Balance: ₹%.2lf\n", acc->balance);
                break;
            case '2':
                depositMoney(acc);
                saveData();
                break;
            case '3':
                withdrawMoney(acc);
                saveData();
                break;
            case '4':
                showAtmCard(acc);
                break;
            case '5':
                showTransactions(acc);
                break;
            case '6':
                animateText("Logging out...");
                saveData();
                return;
            default:
                animateText("Invalid choice.");
        }
    }
}

int main() {
    srand(time(NULL));
    loadData();

    while (1) {
        printf("\n=== Little S.K Bank Mobile Banking ===\n");
        printf("1. Register\n2. Login\n0. Exit\nEnter choice: ");
        char choice[10];
        fgets(choice, sizeof(choice), stdin);

        switch (choice[0]) {
            case '1':
                registerAccount();
                break;
            case '2': {
                Account *acc = loginAccount();
                if (acc) accountMenu(acc);
                break;
            }
            case '0':
                animateText("Thank you for banking with us. Goodbye!");
                exit(0);
            default:
                animateText("Invalid choice.");
        }
    }
    return 0;
}
