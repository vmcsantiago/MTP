/* Software para controle de acesso com Arduíno
Aluno: Vilson Magner Carrijo Santiago
Nº matrícula: 11521EEL011 */

#include <stdio.h>
#include <windows.h>
#include <locale.h>
#include <string.h>
#include <stdlib.h>
#define PADRAO 2108 //Defina a senha do administrador para realizar novos cadastros

void menu() // Função que mostra o menu principal
{
    system("cls");
    printf("------------- Projeto MTP # Controle de Acesso com Arduíno -------------\n\n\n");
    printf("[1] Cadastro de login/senha\n");
    printf("[2] Digitar login/senha para liberar acesso\n");
    printf("[3] Conferir o número de acessos do dia\n");
    printf("[4] Abrir relatório.txt com login/senha dos acessos do dia\n");
    printf("[5] Sair do programa\n\n");
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
int acessos = 0;
FILE *relatorio;

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
    relatorio = fopen("relatoriodeacessos.txt", "a");



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
                fprintf(relatorio, "Usuário: %i - Senha: %i\n",Atual->usuario,Atual->senha);
                fclose(relatorio);
                break;
            }
            Atual = (elemento *)Atual->prox;
        }
    }

    if(encontrou == 1)
    {
        printf("\nAcesso permitido!\n");
        SerialPutc(hCom, '1');
        acessos++;
        system("pause");
    }
    else
    {
        printf("\nAcesso negado!\n");
        SerialPutc(hCom, '0');
        system("pause");
    }
    fclose(relatorio);
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
    char dateStr[9];
    int passadm;
    setlocale(LC_ALL, "Portuguese");
    DCB dcb;
    HANDLE hCom;
    BOOL fSuccess;
    LPCWSTR LpcCommPort = "COM3";

    relatorio = fopen("relatoriodeacessos.txt","w");
    _strdate(dateStr);
    fprintf(relatorio, "Controle de Acesso - Relatório do login/senha dos acessos do dia (mm/dd/aa): %s\n\n",dateStr);
    fclose(relatorio);

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

    while(opcao != 5)
    {
        switch(opcao)
        {
            case 1:
                cadastrar();
                break;
            case 2:
                logar(hCom);
                break;
            case 3:
                system("cls");
                printf("Seja bem-vindo ao controle do número de acessos do dia (mm/dd/aa): %s\n\n",dateStr);
                printf("Hoje tivemos %d acessos!\n",acessos);
                system("pause");
                break;
            case 4:
                printf("Digite a senha do administrador: ");
                scanf("%i",&passadm);
                if(passadm == PADRAO)
                {
                    system("relatoriodeacessos.txt");
                }
                else
                {
                    printf("Você não tem permissão para visualizar o relatório de acessos!\n");
                    system("pause");
                }
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
