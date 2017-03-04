#include<stdio.h>
#include<stdlib.h>


// drzewa, bêd¹ce elementami listy jednokierunkowej
typedef struct DrzewoB {
	struct DrzewoB *lewy, *prawy;
	int ilosc;
	char wartosc;
} DrzewoB;


//elementy listy jednokierunkowej
typedef struct ElemListy {
	struct ElemListy *nast;
	DrzewoB *korzen;
}ElemListy;

//struktura przechowuj¹ca statystyki slow:
typedef struct statystyka {
	int LiczbaPowtorek;
	char wartosc;
}statystyka;

//struktura przechowuj¹ca znaki wraz z ich znakami koduj¹cymi oraz dlugoscia slow kodowych:
typedef struct kodujaca {
	int dlugoscslow;
	char wartosc, *zakodowane;
}kodujaca;


//funkcja zliczajaca statystyke slow:

statystyka* zliczanie(unsigned char* s,int *dl)
{
	unsigned char x;
	int dlug = *dl;
	int i,j,k; //zmienne iteracyjne
	int k1=257; //wielkosc tablicy
	statystyka *wynik, temp;
	
	wynik = (statystyka*)malloc(sizeof(statystyka)*(k1+1));
	
	for(i=0;i<k1;i++)		//wstawianie wartosci poczatkowych do struktury
	{
		wynik[i].LiczbaPowtorek = 0;
		wynik[i].wartosc = i;
	}
	
	FILE *file = fopen(s, "rb");
	if(!file)
		{
			perror("Blad otwarcia pliku");
			exit (1);
		}
	fseek(file, 0, SEEK_END);
	int nbytes = ftell(file);
	unsigned char *buf = malloc(nbytes);
	fseek(file, 0, SEEK_SET);
	fread(buf, 1, nbytes, file);
	fclose(file);
	
	
	for(k=0;k<nbytes;k++)
		wynik[buf[k]].LiczbaPowtorek++;

	free(buf);
	
	//sortowanie babelkowe malejace:	
	for(i=0;i<k1-1;i++)
		for(j=0;j<k1-i-1;j++)
			if(wynik[j].LiczbaPowtorek<wynik[j+1].LiczbaPowtorek)
				{
				temp = wynik[j];
				wynik[j] = wynik[j+1];
				wynik[j+1] = temp;
				}
			
	while(wynik[dlug].LiczbaPowtorek)	//obliczenie niezerowych wartosci
		(dlug)++;
	
	(*dl) = dlug;
	
		
	return wynik;
}

//funkcja dodawania elementu na pocz¹tek listy
int DodajNaPoczatek(ElemListy **pocz, DrzewoB *drzewo)
{
	ElemListy *tmp;
	tmp = (ElemListy*)malloc(sizeof(ElemListy));
	if (tmp == NULL)
		return 1;
	tmp->korzen = drzewo;
	tmp->nast = *pocz;
	*pocz = tmp;
	return 0;
}




//dodawanie elementu:
DrzewoB* DodajDrzewo(int a,char b)
{
	DrzewoB *Drzewo;
	
	Drzewo = (DrzewoB*)malloc(sizeof(DrzewoB));
	Drzewo->ilosc = a;
	Drzewo->wartosc = b;
	Drzewo->lewy = NULL;
	Drzewo->prawy = NULL;
}


//Tworzenie drzewa Huffmana
DrzewoB* StworzDrzewo(ElemListy **pocz)
{
	int i;
	char b = 0;
	DrzewoB *Huffman;
	
	while((*pocz)->nast)
	{
	i=1;						//zmienna potrzebna do operowania pêtl¹
	ElemListy *tmp1,*tmp2;
	DrzewoB *Drzewo;
	tmp2=*pocz;
	Drzewo = DodajDrzewo(tmp2->korzen->ilosc + tmp2->nast->korzen->ilosc,b);
	Drzewo->lewy = tmp2->korzen;
	Drzewo->prawy = tmp2->nast->korzen;
	tmp1 = (ElemListy*)malloc(sizeof(ElemListy));
	tmp1->nast = NULL;
	tmp1->korzen = Drzewo;
	
	if(tmp2->nast->nast)
		{
		*pocz = tmp2->nast->nast;
		free(tmp2->nast);
	free(tmp2);
	tmp2 = *pocz;
	if(tmp2->korzen->ilosc>tmp1->korzen->ilosc)
		{
			tmp1->nast = *pocz;
			*pocz = tmp1;
			i=0;
		}
	while(tmp2->nast && i)
		{
			if(tmp2->nast->korzen->ilosc>=tmp1->korzen->ilosc)
				{
					i=0;
					tmp1->nast = tmp2->nast;
					tmp2->nast = tmp1;	
				}
			else
				tmp2 = tmp2->nast;
		}
		if(i)
			tmp2->nast = tmp1;
		
	}
	else
		*pocz = tmp1;
	}
	Huffman = (DrzewoB*)malloc(sizeof(DrzewoB));
	Huffman = (*pocz)->korzen;
	return Huffman;
}

//sortowanie babelkowe rosnace:

void Sortowanie(kodujaca **k,int k1)
{
	
	kodujaca temp;
	int i,j;
	for(i=0;i<k1-1;i++)
		for(j=0;j<k1-i-1;j++)
			if((*k)[j].dlugoscslow>(*k)[j+1].dlugoscslow)
				{
				temp = (*k)[j];
				(*k)[j] = (*k)[j+1];
				(*k)[j+1] = temp;
				}
}


//Tworzenie tabeli kodowej do kompresji
void StworzTabeleKodowa(DrzewoB *Drzewo,kodujaca **k,char c[], int wielkosc,int *ilosc)
{	(*ilosc)+=2;
	static int nr=0;	
	if(!Drzewo->lewy)
		{
			int i;
			(*k)[nr].zakodowane = (char*)malloc(sizeof(char)*(wielkosc+1));
			(*k)[nr].wartosc = Drzewo->wartosc;
			(*k)[nr].dlugoscslow = wielkosc;
			for(i=0;i<wielkosc;i++)
				(*k)[nr].zakodowane[i] = c[i];
				nr++;
		}
		
		else
		{
			c[wielkosc] = '0';
			StworzTabeleKodowa(Drzewo->lewy,k,c, wielkosc+1,ilosc);
			c[wielkosc] = '1';
			StworzTabeleKodowa(Drzewo->prawy,k,c, wielkosc+1,ilosc);
			
		}
}

//Zapis wezlow PREORDER
void ZapisWezlow(DrzewoB *Drzewko,FILE *str)
{	char i;
	if(!Drzewko->lewy)
	{
		i=Drzewko->wartosc;
		fwrite(&i,sizeof(char),1,str);
	}
	else
	{
	ZapisWezlow(Drzewko->lewy,str);
	ZapisWezlow(Drzewko->prawy,str);
	}	
}

//Zapis Drzewa Huffmana do pliku: PREORDER. Jeœli wskaŸnik nie jest NULL wpisujemy 1, w przeciwnym przypadku 0
void Preorder1(DrzewoB *Drzewo,FILE *str,int ilosc)
	{	
		static char zapis = 0;
		static int i = 0;
		static int k = 0;
		
		if(Drzewo)
			{
				if(Drzewo->lewy)
					{
					if(i>6)
						{
							i=0;
							zapis = zapis | 1;
							fwrite(&zapis,sizeof(char),1,str);
							zapis = 0;
						}
					else
						{
							i++;
							zapis = zapis | 1;
							zapis = zapis<<1;
						}
						Preorder1(Drzewo->lewy,str,ilosc);
					}
				else
					if(i>6)
						{
							i=0;
							fwrite(&zapis,sizeof(char),1,str);
							zapis = 0;
						}
					else
						{
							i++;
							zapis = zapis<<1;
						}
				if(Drzewo->prawy)
					{
					if(i>6)
						{
							i=0;
							zapis = zapis | 1;
							fwrite(&zapis,sizeof(char),1,str);
							zapis = 0;
						}
					else
						{
							i++;
							zapis = zapis | 1;
							zapis = zapis<<1;
						}
					Preorder1(Drzewo->prawy,str,ilosc);
					}
				else
					if(i>6)
						{
							i=0;
							fwrite(&zapis,sizeof(char),1,str);
							zapis = 0;
						}
					else
						{
							i++;
							zapis = zapis<<1;
						}	
					}
			k=k+2;
			if(!(k-ilosc))
				if(i)
					{
						zapis = zapis<<(8-i-1);
						fwrite(&zapis,sizeof(char),1,str);
					}
					
}


void ZapisKompresji(DrzewoB *Drzewo,char *s2,char *s1,int dl,kodujaca **k,int ilosc)
{
	
	int i=50,iter1,iter2,wart,osiem = 0,iter3,j;
	char bufor[50],znak;
	long IloscBajtowPliku = (long)Drzewo->ilosc;
	FILE *wy;
	wy = fopen(s2,"wb");
	
	fwrite(&ilosc,sizeof(int),1,wy);				//zapis podwojonej ilosci wezlow
	
	Preorder1(Drzewo,wy,ilosc);										//zapis drzewa PREORDER
	
	fwrite(&dl,sizeof(int),1,wy);								//zapis ilosci wezlow Drzewa Huffmana
	ZapisWezlow(Drzewo,wy);										//Zapis slow kodowych PREORDER
	
	fwrite(&IloscBajtowPliku,sizeof(long),1,wy);						//Zapis ilosci bitow kodu

	//Zapis bitów kodu
	
	FILE *we;
	we = fopen(s1,"rb");
	if(!we)
		{
			perror("Blad otwarcia pliku");
			exit (1);
		}
	int r = 0;
	int p = 0;

	while(IloscBajtowPliku+i>i)
		{	if(IloscBajtowPliku>i)
				{
				fread(bufor,sizeof(char),sizeof(bufor),we);
				
				j=i;
				}
				else
				{
				fread(bufor,sizeof(char),IloscBajtowPliku,we);
				j=IloscBajtowPliku;
				
				}
			IloscBajtowPliku = IloscBajtowPliku - i;
			
			for(iter1 = 0;iter1<j;iter1++)
					{
						p++;
						wart = 1;
						iter2 = 0;
				while(iter2-dl && wart)
					if((*k)[iter2].wartosc == bufor[iter1])
						{
							
							wart=0;
						for(iter3=0;iter3<(*k)[iter2].dlugoscslow;iter3++)
							{
							osiem++;
							r++;
							if((*k)[iter2].zakodowane[iter3]=='1')
									{	znak = znak<<1;
										znak = znak | 1;
									}
									else
										znak = znak<<1;
							if(osiem > 7)
								{
								osiem=0;
								fwrite(&znak,sizeof(char),1,wy);
								}	
							}
						}
					else
						iter2++;
					}
		}
		
		if(osiem)
			{
				
				znak = znak<<(8-osiem);
				fwrite(&znak,sizeof(char),1,wy);
			}

	fclose(we);
	fclose(wy);

}


//////////////////////////////////////////////////////////////////////////////////////////////
//funkcje dekompresji


void OdczytElementuZDrzewa(FILE *str1,FILE *str2,DrzewoB* Drzewo,int t)
		{
			static unsigned char zapis = 0;
			static int i = 8;			//do strumienia str1
			static int k = 128;
				if(i>7)
					{
						fread(&zapis,sizeof(char),1,str1);
						i=0;
						k=128;
					}
				if(!Drzewo->lewy)
					fwrite(&(Drzewo->wartosc),sizeof(char),1,str2);
				else
					if(zapis>=k)
						{
							i++;
							zapis = zapis - k;
							k = k/2;
							OdczytElementuZDrzewa(str1,str2,Drzewo->prawy,t);
						}
						else
						{	
							i++;
							k = k/2;
							OdczytElementuZDrzewa(str1,str2,Drzewo->lewy,t);
						}
		}

void OdtworzCaleDrzewo(DrzewoB *Drzewo,char *s)
	{
		static int licznik=0;
		
		if(!Drzewo->lewy)
			{
				Drzewo->wartosc = s[licznik];
				licznik++;
			}
		else
		{
			OdtworzCaleDrzewo(Drzewo->lewy,s);
			OdtworzCaleDrzewo(Drzewo->prawy,s);
		}
	}

char* OdczytajStruktureDrzewa(FILE *str,int licznik)
	{
		int i,k,j=0;
		unsigned char x;
		char *tab;
		tab = (char*)malloc(sizeof(char)*(licznik+1));
		
		while(licznik)
		{	
		fread(&x,sizeof(char),1,str);
		k = 256;
		for(i=0;i<8;i++)
			if(licznik)
				{
					k = k/2;
					if((x-k)>=0)
						{
						tab[j] = '1';
						x = x-k;
						}
					else
						tab[j] = '0';
					licznik--;
					j++;
				}
		}
		return tab;
	}


void OdtworzDrzewo(DrzewoB *Drzewo,int liczba,char *wezly)
	{
		static int licznik = 0;
		if(licznik<liczba)
		{
		if(wezly[licznik] == '1')
			{
				licznik++;
				Drzewo->lewy = DodajDrzewo(0,0);
				OdtworzDrzewo(Drzewo->lewy,liczba,wezly);
			}
			else
			licznik++;
		if(wezly[licznik] == '1')
			{
				licznik++;
				Drzewo->prawy = DodajDrzewo(0,0);
				OdtworzDrzewo(Drzewo->prawy,liczba,wezly);
			}
			else
			licznik++;
		}
	}

DrzewoB* UsunDrzewo(DrzewoB *Drzewo)
	{	if(Drzewo != NULL)
		{
		if(Drzewo->lewy == NULL && Drzewo->prawy == NULL)
				{
				free(Drzewo);
				return NULL;
				}
		else
			{
			Drzewo->lewy = UsunDrzewo(Drzewo->lewy);
			Drzewo->prawy = UsunDrzewo(Drzewo->prawy);
			}
		}
		return Drzewo;
	}

void Dekompresja(char *we,char *wy)
	{
		int i;
		FILE *strumienWe;
		FILE *strumienWy;
		
		strumienWe = fopen(we,"rb");
		if(!strumienWe)
		{
			perror("Blad otwarcia pliku");
			exit (1);
		}
		strumienWy = fopen(wy,"wb");
		DrzewoB *KorzenHuffmana2;
		KorzenHuffmana2 = DodajDrzewo(0,0);
		
		int PodwIlWezlow;
		fread(&PodwIlWezlow,sizeof(int),1,strumienWe);				//odczytanie z pliku wartoœci typu int okreœlaj¹cej liczbê bitów s³u¿¹cych do zapisania drzewa Huffmana
		
		char *Wezly;
		Wezly = OdczytajStruktureDrzewa(strumienWe,PodwIlWezlow);	//Zapisanie bitów jako '0' lub '1' w tablicy znaków
		OdtworzDrzewo(KorzenHuffmana2,PodwIlWezlow,Wezly);		//Odtworzenie drzewa Huffmana; na razie bez wpisanych odpowiednich wartoœci
		
		int IloscSlowKodowych2;
		fread(&IloscSlowKodowych2,sizeof(int),1,strumienWe);		//odczyt wartoœci typu int okreœlaj¹cej iloœæ s³ów kodowych
		char SlowaKodowe[IloscSlowKodowych2+1];
		for(i=0;i<IloscSlowKodowych2;i++)
				fread(&SlowaKodowe[i],sizeof(char),1,strumienWe);				//zapis s³ów kodowych w tablicy SlowaKodowe
			
			
		OdtworzCaleDrzewo(KorzenHuffmana2,SlowaKodowe);						//zapis wartosci do drzewa
		long IloscBajtowKompresji;		
		fread(&IloscBajtowKompresji,sizeof(long),1,strumienWe);					//odczyt ilosci bajtow w pliku przed skompresowaniem
		while(IloscBajtowKompresji)
			{
				
			OdczytElementuZDrzewa(strumienWe,strumienWy,KorzenHuffmana2,IloscBajtowKompresji);			//funkcja odczytuje bity i przechodz¹c po drzewie (0 - w lewo, 1 - w prawo)						
			IloscBajtowKompresji = IloscBajtowKompresji - 1;											//wczytuje do pliku wyjsciowego wartosc, która jest w lisciu drzewa
			}
		
		fclose(strumienWe);
		fclose(strumienWy);
		KorzenHuffmana2 = UsunDrzewo(KorzenHuffmana2);									//usuwanie drzewa
		free(Wezly);
		
	}
		
	Kompresja(char *we, char *wy)
		{
			int i;										//zmienna iteracyjna
			int dlugosc=0;								//dlugosc tablicy slow (ilosc slow kodowych)
			int PodwojonaIloscWezlow = 0;				//podwojona iloœæ wezlow drzewa Huffmana
			char test[40];
			kodujaca *ptrkod;
			ElemListy *poczatek = NULL;					//wskaŸnik na pocz¹tek tablicy
			DrzewoB *Drzewko, *KorzenHuffmana;			
			statystyka *tablica;
			tablica = zliczanie(we,&dlugosc);							//statystyka slow
			for(i=0;i<dlugosc;i++)										//tworzenie listy korzeni
				{
			Drzewko = DodajDrzewo(tablica[i].LiczbaPowtorek,tablica[i].wartosc);
			DodajNaPoczatek(&poczatek,Drzewko);	
				}
		
			KorzenHuffmana = StworzDrzewo(&poczatek);					//utworzenie drzewa Huffmana
			
			
			ptrkod = (kodujaca*)malloc(sizeof(kodujaca)*dlugosc);
			
			StworzTabeleKodowa(KorzenHuffmana,&ptrkod,test, 0,&PodwojonaIloscWezlow);			//utworzenie tabeli kodowej
			Sortowanie(&ptrkod,dlugosc);								//sortowanie tabeli kodowej po dlugosci slow kodowych
			ZapisKompresji(KorzenHuffmana,wy,we,dlugosc,&ptrkod,PodwojonaIloscWezlow);		//tworzy kompresjê Huffmana
			free(tablica);
			free(ptrkod);
			KorzenHuffmana = UsunDrzewo(KorzenHuffmana);
		}


	
main(int args,char *argv[])
{
	char s;
	printf("Kompresja - k, dekompresja - d, inny znak - brak dzialania:\n");
	scanf("%c",&s);
	if(s == 'k')
	Kompresja(argv[1],argv[2]);
	if(s == 'd')
	Dekompresja(argv[1],argv[2]);
	return 0;
}
