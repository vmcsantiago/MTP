/* Software para controle de acesso com Arduíno
Aluno: Vilson Magner Carrijo Santiago
Nº matrícula: 11521EEL011 */

#include <stdio.h>
#include <windows.h>
#include <locale.h>
#include <string.h>
#define PADRAO 2108 //Defina a senha do administrador para realizar novos cadastros

void menu() // Função que mostra o menu principal
{
    system("cls");
    printf("------------- Projeto MTP # Controle de Acesso com Arduíno -------------\n\n\n");
    printf("[1] Cadastro de login/senha\n");
    printf("[2] Digitar login/senha para liberar acesso\n");
    printf("[3] Sair do programa\n\n");
    printf("Opção desejada: ");

}

void SerialPutc(HANDLE hCom, char txchar)
{
	BOOL bWriteRC;
	static DWORD iBytesWritten;

	bWriteRC = WriteFile(hCom, &txchar, 1, &iBytesWritten,NULL);

	return;
}

typedef struct
{
    int usuario;
    int senha;
    struct elemento *prox;
}elemento;

elemento *F = NULL;
elemento *Atual = NULL;
elemento *Anterior = NULL;

elemento *alocarNo()
{
    elemento *novoNo;
    novoNo = (elemento *)malloc(sizeof(elemento));
    novoNo->prox = NULL;
    return novoNo;
}

void conferir(int user, int pass, HANDLE hCom)
{
    int encontrou = 0;

    if(F==NULL) //FILA VAZIA
    {
        printf("Nenhum usuário cadastrado!\n");
    }
    else // FILA NÃO ESTÁ VAZIA
    {
        Atual = F;

        while(Atual!=NULL)
        {
            if(Atual->usuario == user && Atual->senha == pass)
            {
                encontrou = 1;
            }
            Atual = (elemento *)Atual->prox;
        }
    }

    if(encontrou == 1)
    {
        printf("\nAcesso permitido!\n");
        SerialPutc(hCom, '1');
        system("pause");
    }
    else
    {
        printf("\nAcesso negado!\n");
        system("pause");
    }
}

void logar(HANDLE hCom)
{
    system("cls");
    int user;
    int pass;

    printf("Seja bem-vindo ao login para o acesso!\n\n");
    printf("Insira o usuário: ");
    scanf("%d",&user);
    printf("Insira a senha: ");
    scanf("%d",&pass);
    conferir(user, pass, hCom);

}

void cadastrar()
{
    system("cls");
    int op;
    int user;
    int pass;

    printf("Seja bem-vindo ao cadastro para o acesso!\n\n");
    printf("Insira a senha do administrador para realizar um novo cadastro: ");
    scanf("%d",&op);

    if(op == PADRAO)
    {
        printf("Digite o usuário (Numérico): ");
        scanf("%d",&user);
        printf("Digite a senha (Numérico): ");
        scanf("%d",&pass);

        if(F == NULL) // Fila vazia
        {
            F = alocarNo();
            F->usuario = user;
            F->senha = pass;
        }
        else // Fila não está vazia
        {
            Atual = F;

            if(Atual->prox == NULL) //Possui apenas 1 elemento
            {
                Atual->prox = (struct elemento *)alocarNo();
                Atual = (elemento *)Atual->prox;
                Atual->usuario = user;
                Atual->senha = pass;
            }
            else // Possui mais de 1 elemento
            {
                while(Atual->prox!=NULL)
                {
                    Atual = (elemento *)Atual->prox;
                }
                Atual->prox = (struct elemento *)alocarNo();
                Atual = (elemento *)Atual->prox;
                Atual->usuario = user;
                Atual->senha = pass;
            }
        }
    }
    else
        printf("Você não tem permissão para efetuar um novo cadastro!\n");

    system("pause");
}

void main() // Programa principal
{
    int opcao;
    setlocale(LC_ALL, "Portuguese");
    DCB dcb;
    HANDLE hCom;
    BOOL fSuccess;
    LPCWSTR LpcCommPort = "COM3";

    hCom = CreateFile( LpcCommPort,
                    GENERIC_READ | GENERIC_WRITE,
                    0,    // must be opened with exclusive-access
                    NULL, // no security attributes
                    OPEN_EXISTING, // must use OPEN_EXISTING
                    0,    // not overlapped I/O
                    NULL  // hTemplate must be NULL for comm devices
                    );

   if (hCom == INVALID_HANDLE_VALUE)
   {
       // Handle the error.
       printf ("CreateFile failed with error %d.\n", GetLastError());
       return (1);
   }

   // Build on the current configuration, and skip setting the size
   // of the input and output buffers with SetupComm.

   fSuccess = GetCommState(hCom, &dcb);

   if (!fSuccess)
   {
      // Handle the error.
      printf ("GetCommState failed with error %d.\n", GetLastError());
      return (2);
   }

   // Fill in DCB: 57,600 bps, 8 data bits, no parity, and 1 stop bit.

   dcb.BaudRate = CBR_9600;     // set the baud rate
   dcb.ByteSize = 8;             // data size, xmit, and rcv
   dcb.Parity = NOPARITY;        // no parity bit
   dcb.StopBits = ONESTOPBIT;    // one stop bit

   fSuccess = SetCommState(hCom, &dcb);

   //

   if (!fSuccess)
   {
      // Handle the error.
      printf ("SetCommState failed with error %d.\n", GetLastError());
      return (3);
   }
    menu();
    scanf("%d",&opcao);

    while(opcao != 3)
    {
        switch(opcao)
        {
            case 1:
                cadastrar();
                break;
            case 2:
                logar(hCom);
                break;
            default:
                printf("Opção nao cadastrada, por favor, tente novamente!\n");
                system("pause");
        }
        menu();
        scanf("%i",&opcao);
    }
    CloseHandle(hCom);
    system("pause");
}
