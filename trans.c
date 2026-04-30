// Bank-account program reads a random-access file sequentially,
// updates data already written to the file, creates new data to
// be placed in the file, and deletes data previously in the file.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// clientData structure definition
struct clientData
{
    unsigned int acctNum; // account number
    char lastName[15];    // account last name
    char firstName[10];   // account first name
    double balance;       // account balance
};                        // end structure clientData

// prototypes
unsigned int enterChoice(void);
void createFileIfMissing(const char *fileName);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void listRecords(FILE *fPtr);
void accountDetails(FILE *fPtr);
int readAccount(unsigned int account, struct clientData *client, FILE *fPtr);
void writeAccount(const struct clientData *client, unsigned int account, FILE *fPtr);

int main(int argc, char *argv[])
{
    FILE *cfPtr;         // credit.dat file pointer
    unsigned int choice; // user's choice

    if (argc < 1)
    {
        printf("Program name is unavailable.\n");
        return EXIT_FAILURE;
    }

    createFileIfMissing("credit.dat");

    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
    {
        printf("%s: File could not be opened.\n", argv[0]);
        return EXIT_FAILURE;
    }

    while ((choice = enterChoice()) != 7)
    {
        switch (choice)
        {
        case 1:
            textFile(cfPtr);
            break;
        case 2:
            updateRecord(cfPtr);
            break;
        case 3:
            newRecord(cfPtr);
            break;
        case 4:
            deleteRecord(cfPtr);
            break;
        case 5:
            listRecords(cfPtr);
            break;
        case 6:
            accountDetails(cfPtr);
            break;
        default:
            puts("Incorrect choice. Please enter a number between 1 and 7.");
            break;
        }
    }

    fclose(cfPtr);
    puts("Program ended.");
    return EXIT_SUCCESS;
}

void createFileIfMissing(const char *fileName)
{
    FILE *filePtr = fopen(fileName, "rb");
    if (filePtr != NULL)
    {
        fclose(filePtr);
        return;
    }

    filePtr = fopen(fileName, "wb+");
    if (filePtr == NULL)
    {
        puts("Unable to create the data file.");
        exit(EXIT_FAILURE);
    }

    struct clientData blankClient = {0, "", "", 0.0};
    for (unsigned int i = 0; i < 100; ++i)
    {
        fwrite(&blankClient, sizeof(struct clientData), 1, filePtr);
    }
    fclose(filePtr);
}

void textFile(FILE *readPtr)
{
    FILE *writePtr;
    struct clientData client = {0, "", "", 0.0};

    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        puts("accounts.txt could not be opened.");
        return;
    }

    rewind(readPtr);
    fprintf(writePtr, "%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");
    fprintf(writePtr, "%-6s%-16s%-11s%10s\n", "-----", "---------", "----------", "----------");

    while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            fprintf(writePtr, "%-6u%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName,
                    client.balance);
        }
    }

    fclose(writePtr);
    puts("accounts.txt has been created.");
}

void updateRecord(FILE *fPtr)
{
    unsigned int account;
    struct clientData client = {0, "", "", 0.0};

    printf("Enter account to update (1 - 100): ");
    if (scanf("%u", &account) != 1 || account < 1 || account > 100)
    {
        puts("Invalid account number.");
        return;
    }

    if (!readAccount(account, &client, fPtr))
    {
        printf("Account #%u has no information.\n", account);
        return;
    }

    printf("Current record:\n");
    printf("%-6u%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);

    double transaction;
    printf("Enter charge (+) or payment (-): ");
    if (scanf("%lf", &transaction) != 1)
    {
        puts("Invalid amount.");
        return;
    }

    client.balance += transaction;
    writeAccount(&client, account, fPtr);
    printf("Updated record:\n");
    printf("%-6u%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);
}

void deleteRecord(FILE *fPtr)
{
    unsigned int accountNum;
    struct clientData client = {0, "", "", 0.0};
    struct clientData blankClient = {0, "", "", 0.0};

    printf("Enter account number to delete (1 - 100): ");
    if (scanf("%u", &accountNum) != 1 || accountNum < 1 || accountNum > 100)
    {
        puts("Invalid account number.");
        return;
    }

    if (!readAccount(accountNum, &client, fPtr))
    {
        printf("Account %u does not exist.\n", accountNum);
        return;
    }

    writeAccount(&blankClient, accountNum, fPtr);
    printf("Account %u has been deleted.\n", accountNum);
}

void newRecord(FILE *fPtr)
{
    unsigned int accountNum;
    struct clientData client = {0, "", "", 0.0};

    printf("Enter new account number (1 - 100): ");
    if (scanf("%u", &accountNum) != 1 || accountNum < 1 || accountNum > 100)
    {
        puts("Invalid account number.");
        return;
    }

    if (readAccount(accountNum, &client, fPtr) && client.acctNum != 0)
    {
        printf("Account #%u already contains information.\n", client.acctNum);
        return;
    }

    printf("Enter lastname, firstname, balance\n? ");
    if (scanf("%14s %9s %lf", client.lastName, client.firstName, &client.balance) != 3)
    {
        puts("Invalid input. Please enter lastname firstname balance.");
        return;
    }

    client.acctNum = accountNum;
    writeAccount(&client, accountNum, fPtr);
    printf("Account #%u has been created.\n", accountNum);
}

void listRecords(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};
    unsigned int count = 0;

    rewind(fPtr);
    printf("%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");
    printf("%-6s%-16s%-11s%10s\n", "-----", "---------", "----------", "----------");

    while (fread(&client, sizeof(struct clientData), 1, fPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            printf("%-6u%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName,
                   client.balance);
            ++count;
        }
    }

    if (count == 0)
    {
        puts("No accounts found.");
    }
}

void accountDetails(FILE *fPtr)
{
    unsigned int account;
    struct clientData client = {0, "", "", 0.0};

    printf("Enter account number to view (1 - 100): ");
    if (scanf("%u", &account) != 1 || account < 1 || account > 100)
    {
        puts("Invalid account number.");
        return;
    }

    if (!readAccount(account, &client, fPtr) || client.acctNum == 0)
    {
        printf("Account #%u has no information.\n", account);
        return;
    }

    printf("Account details:\n");
    printf("Acct: %u\nLast Name: %s\nFirst Name: %s\nBalance: %.2f\n",
           client.acctNum, client.lastName, client.firstName, client.balance);
}

int readAccount(unsigned int account, struct clientData *client, FILE *fPtr)
{
    if (account < 1 || account > 100)
    {
        return 0;
    }

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    return fread(client, sizeof(struct clientData), 1, fPtr) == 1;
}

void writeAccount(const struct clientData *client, unsigned int account, FILE *fPtr)
{
    if (account < 1 || account > 100)
    {
        return;
    }

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(client, sizeof(struct clientData), 1, fPtr);
}

unsigned int enterChoice(void)
{
    unsigned int menuChoice;
    printf("%s", "\nEnter your choice\n"
                 "1 - store a formatted text file of accounts called\n"
                 "    \"accounts.txt\" for printing\n"
                 "2 - update an account\n"
                 "3 - add a new account\n"
                 "4 - delete an account\n"
                 "5 - list all accounts\n"
                 "6 - view account details\n"
                 "7 - end program\nChoice: ");

    if (scanf("%u", &menuChoice) != 1)
    {
        menuChoice = 0;
    }
    return menuChoice;
} // end function enterChoice