#include <Windows.h>
#include <winspool.h>
#include <string>
#include <fstream>
#include <iostream>
#include <tchar.h>
#include <strsafe.h>
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"advapi32.lib")

using namespace std;

class cFuncoes
{
private:

	//Nesta foi usada o nível 2 de busca das informações, mas poderá alterar.
	PRINTER_INFO_2* Impressora;

public:

	void EnumerarImpressorasLocais()
	{
		DWORD BytesRetornados;
		DWORD Retornados = 0;
		EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, 0, 2, 0, 0, &BytesRetornados, &Retornados);

		Impressora = (PRINTER_INFO_2*)malloc(BytesRetornados);

		Retornados = 0;
		EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, 0, 2, (LPBYTE)Impressora, BytesRetornados, &BytesRetornados, &Retornados);

		int Quantidade = 0;
		for (int i = 0; i < Retornados; i++)
		{
			Quantidade += 1;
			_tprintf(L"Impressora: %s\n", Impressora[i].pPrinterName);
			_tprintf(L"Driver: %s\nPorta: %s\n\n", Impressora[i].pDriverName, Impressora[i].pPortName);
		}

		cout << "Quantidade: " << Quantidade << " encontradas\n\n";

		//Iremos liberar a memória alocada na matriz.
		free(Impressora);
	}

	void ObterImpressoraPadrao()
	{
		TCHAR tImpressora[MAX_PATH];
		DWORD Tamanho = sizeof(tImpressora);
		GetDefaultPrinter(tImpressora, &Tamanho);

		_tprintf(L"Impressora padrão: %s\n", tImpressora);
	}

	void Imprimir(const wchar_t* Impressora, const wchar_t* Arquivo)
	{
		AddPrinterConnection((LPWSTR)Impressora);
		
		HANDLE Identificador;
		
		PRINTER_DEFAULTS Parametros;
		Parametros.DesiredAccess = PRINTER_ALL_ACCESS;
		Parametros.pDatatype = 0;
		Parametros.pDevMode = 0;
		OpenPrinter((LPWSTR)Impressora, &Identificador, &Parametros);

		TCHAR Documento[MAX_PATH];
		TCHAR Dados[MAX_PATH];

		fstream fst;
		fst.open(Arquivo);
		if (fst.is_open() == true)
		{
			string Texto;

			while (getline(fst,Texto))
			{
				StringCchCopy(Documento, Texto.length(), (STRSAFE_LPCWSTR)Texto.c_str());

				DOC_INFO_1 Doc;
				Doc.pDocName = Documento;
				Doc.pOutputFile = 0;
				Doc.pDatatype = 0;
				StartDocPrinter(Identificador, 1, (LPBYTE)&Doc);
				StartPagePrinter(Identificador);

				DWORD Escritos;
				WritePrinter(Identificador, Documento, Texto.length(), &Escritos);

				EndPagePrinter(Identificador);
				EndDocPrinter(Identificador);
			}
		}
		else
		{
			cout << "Não foi possível obter a leitura do arquivo..\n";
		}

		//Iremos fechar o identificador da impressora pois não será mais necessário após os procedimentos feitos.
		ClosePrinter(Identificador);
	}

	void RemoverImpressora(const wchar_t* wImpressora)
	{
		HANDLE Identificador;

		PRINTER_DEFAULTS Parametros;
		Parametros.DesiredAccess = PRINTER_ALL_ACCESS;
		Parametros.pDatatype = 0;
		Parametros.pDevMode = 0;
		OpenPrinter((LPWSTR)wImpressora, &Identificador, &Parametros);

		DeletePrinter(Identificador);
	}

}Funcoes;

int main()
{

	cout << "O assistente está executando operações de impressoras no computador local...\n";

	Funcoes.EnumerarImpressorasLocais();
	Funcoes.ObterImpressoraPadrao();
	Funcoes.Imprimir(L"Nome da impressora", L"Seu arquivo de texto");
	Funcoes.RemoverImpressora(L"Nome da impressora");

	system("pause");
}