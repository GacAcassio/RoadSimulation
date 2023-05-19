#include <stdio.h>
#include <stdlib.h>
#include<time.h>
#include "structs.h"
#define varEntrada 6
#define fixEntrada 9
#define variacaoVeiculos 100
#define numeroTestes 1000
#define tempoSemaforo 15
//Cria um tipo abstrato de dados para Veículo
typedef struct
{
  int tipo, colisao, preferencial;
  double tempo;
}Veiculo;

//Cria uma abstração de dados 
typedef struct
{
  Veiculo veiculos[600];
  int inicio, fim, max;
  int quantCarros, quantPreferenciais, quantOnibus;
  //Booleanos para facilitar a detecção de veículos prefenciais e se a pista ainda tem(ou terá) carros
  int prefenciais, abilitada;
}Via;

//Cria uma abstração para rotatória com 3 vias
typedef struct
{
  Via vias[3];
}Rotatoria;

//Vetor global para guardar resultados
float tabela[numeroTestes][4];
int indexTabela;
int flag = 0;
int valid = 1;

void acrescimoTempo(Rotatoria* rotatoria, int tempo, int pref, int sem);
void acrescimoBatida(Rotatoria* rotatoria);
double calculaTempo(Rotatoria* rotatoria);
int colisao(int riscoColisao);
void chegadaVeiculo();
int getEntrada();
int getPreferencialOuTipo();
int getMax();
int getPista();
int getSaida(Rotatoria* rotatoria, int via, int semaforo, int preferencial);
void getVeiculos(Rotatoria* rotatoria, int via, int sem);
void imprimeResultado();
void iniciaRotatoria(Rotatoria* rotatoria, int n);
void limpaRotatoria(Rotatoria* rotatoria, int i);
void simulaCasoAtual();
void simulaCasoPreferencial();
void simulaCasoSemaforo();
void simulaCasoPrefESema();
void simulaTransito();
void verificaPreferenciais(Rotatoria* rotatoria);

int main(void) {
  srand(time(NULL));
  for(int i = 0; i < numeroTestes; i++)
  {
    indexTabela = i;
    simulaTransito();
    fflush(stdin);
  }
  imprimeResultado();
  return 0;
}

//Adiciona tempo de espera em outros veículos após veículos deixarem a rotatória
void acrescimoTempo(Rotatoria* rotatoria, int tempo, int pref, int sem)
{
  for(int i = 0; i < 3; i++)
  {
    for(int j = rotatoria->vias[i].inicio; j < rotatoria->vias[i].fim; j++)
    {
      //Carros preferenciais não possuem acréscimo, pois supõem-se que já deixaram a pista
      if(! rotatoria->vias[i].veiculos[j].preferencial)
      {
        if(sem && pref)
        {
          if(rotatoria->vias[i].veiculos[j].tipo == 0)
          {
             rotatoria->vias[i].veiculos[j].tempo += tempoSemaforo;
          }  
        }
        else if(sem)
        {
          rotatoria->vias[i].veiculos[j].tempo += tempoSemaforo;
        }
        else if(pref)
        {
          if(rotatoria->vias[i].veiculos[j].tipo == 0)
          {
             rotatoria->vias[i].veiculos[j].tempo += (float) tempo;
          }  
        }
        else
        {
            rotatoria->vias[i].veiculos[j].tempo += (float) tempo;
        }  
      }
    }
  }
}

//Acrescenta o tempo em caso de colisões
void acrescimoBatida(Rotatoria* rotatoria)
{
  float tempo;
  for(int i = 0; i < 3; i++)
  {
    for(int j = rotatoria->vias[i].inicio; j < rotatoria->vias[i].fim; j++)
    {
      tempo = rand() % 11 + 20;
      tempo = tempo / 100;
      //Carros preferenciais não possuem acréscimo, pois supõem-se que já deixaram a pista
      if(rotatoria->vias[i].veiculos[j].preferencial == 0)
      {
        rotatoria->vias[i].veiculos[j].tempo *= (1 + tempo);
      }
    }
  }
}

//Ao final de um teste calula-se o tempo
double calculaTempo(Rotatoria* rotatoria)
{
  double tempo = 0, som = 0;
  for(int i = 0; i < 3; i++)
  {
    for(int j = 0; j < rotatoria->vias[i].fim; j++)
    {
     tempo += rotatoria->vias[i].veiculos[j].tempo;
    }
    som += rotatoria->vias[i].max;
  }
  tempo = tempo / som;
  return tempo;
}
//Popula o atributo colisão das instâncias de veículo, usando o inteiro colisão como um booleano
int colisao(int riscoColisao)
{
  if((rand() % 1000) < riscoColisao)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

//Auxilia a entrada de veículos
int getEntrada()
{
  return ((rand() % varEntrada) + fixEntrada);
}

//Randômico para saber se o carro é Preferencial ou não, ou o tipo(carro 0 ou ônibus 1);
int getPreferencialOuTipo()
{
  return (rand() % 2);
}

//Define o número de referência para testes em uma via como sendo 450 ± variacaoVeiculos
int getMax()
{
  if(rand() % 2)
  {
    return (500 + (rand() % variacaoVeiculos) + 1);
  }
  else
  {
    return (500 - (rand() % variacaoVeiculos) - 1);
  }
}

//Seleciona uma pista randômica
int getPista()
{
  return (rand() % 3);
}

//Determina a quantidade de carros que saíra de uma pista, indo de 1 a 6;
int getSaida(Rotatoria* rotatoria, int via, int semaforo, int preferencial)
{
  int inicio = rotatoria->vias[via].inicio, fim = rotatoria->vias[via].fim;
  int saida;
  if (semaforo && preferencial)
  {
    
    if(rotatoria->vias[via].abilitada)
    {
      //saem de uma pista entre 1 e 6 veiculos;
      int x;
      if(rotatoria->vias[via].inicio == rotatoria->vias[via].fim )
      {
        x = 0;
      }
      else
      {
        x = 1;
      }
      saida = rand() % 6 + 10 + inicio;
      if(saida > fim)
      {
        saida = rotatoria->vias[via].fim;
      }
      //O tempo defaullt que será acrescido nos outros carros será de 1(tempo de espera para passagem de um carro);
      for(int i = inicio; i < saida; i++)
      {
        if(rotatoria->vias[via].veiculos[i].colisao)
        {
          acrescimoBatida(rotatoria);
        }
        if((rotatoria->vias[via].veiculos[i].tipo || rotatoria->vias[via].veiculos[i].preferencial) && saida < rotatoria->vias[via].fim)
        {
          saida++;
        }
      }
      rotatoria->vias[via].inicio = saida;
      if (x)
      {
        acrescimoTempo(rotatoria, 1, 1, 1);
      } 
      if(rotatoria->vias[via].inicio >= rotatoria->vias[via].max)
      {
         rotatoria->vias[via].abilitada = 0;
      }
    }
    
  }
  else if(semaforo)
  {
    if(rotatoria->vias[via].abilitada)
    {
      //Os preferenciais são sempre tratados com prioridade
      verificaPreferenciais(rotatoria);
      //saem de uma pista entre 1 e 6 veiculos;
      int x;
      if(rotatoria->vias[via].inicio == rotatoria->vias[via].fim )
      {
        x = 0;
      }
      else
      {
        x = 1;
      }
      saida = rand() % 6 + 10 + inicio;
      if(saida > fim)
      {
        saida = rotatoria->vias[via].fim;
      }
      //O tempo defaullt que será acrescido nos outros carros será de 1(tempo de espera para passagem de um carro);
      int tempo = 1;
      for(int i = inicio; i < saida; i++)
      {
        if(rotatoria->vias[via].veiculos[i].colisao)
        {
          acrescimoBatida(rotatoria);
        }
        if(rotatoria->vias[via].veiculos[i].tipo)
        {
          tempo = 2;
        }
      }
      rotatoria->vias[via].inicio = saida;
      if (x)
      {
        acrescimoTempo(rotatoria, tempo, 0, 1);
      } 
      if(rotatoria->vias[via].inicio >= rotatoria->vias[via].max)
      {
         rotatoria->vias[via].abilitada = 0;
      }
    }
  }
  else if(preferencial)
  {
    if(rotatoria->vias[via].abilitada)
    {
      //saem de uma pista entre 1 e 6 veiculos;
      int x;
      if(rotatoria->vias[via].inicio == rotatoria->vias[via].fim )
      {
        x = 0;
      }
      else
      {
        x = 1;
      }
      saida = rand() % 6 + 1 + inicio;
      if(saida > fim)
      {
        saida = rotatoria->vias[via].fim;
      }
      //O tempo defaullt que será acrescido nos outros carros será de 1(tempo de espera para passagem de um carro);
      for(int i = inicio; i < saida; i++)
      {
        if(rotatoria->vias[via].veiculos[i].colisao)
        {
          acrescimoBatida(rotatoria);
        }
        if(rotatoria->vias[via].veiculos[i].tipo || rotatoria->vias[via].veiculos[i].preferencial && saida < rotatoria->vias[via].fim)
        {
          saida++;
        }
      }
      rotatoria->vias[via].inicio = saida;
      if (x)
      {
        acrescimoTempo(rotatoria, 1, 1, 0);
      } 
      if(rotatoria->vias[via].inicio >= rotatoria->vias[via].max)
      {
         rotatoria->vias[via].abilitada = 0;
      }
    }
  }
  else
  {
    if(rotatoria->vias[via].abilitada)
    {
      //Os preferenciais são sempre tratados com prioridade
      verificaPreferenciais(rotatoria);
      //saem de uma pista entre 1 e 6 veiculos;
      int x;
      if(rotatoria->vias[via].inicio == rotatoria->vias[via].fim )
      {
        x = 0;
      }
      else
      {
        x = 1;
      }
      saida = rand() % 6 + 1 + inicio;
      if(saida > fim)
      {
        saida = rotatoria->vias[via].fim;
      }
      //O tempo defaullt que será acrescido nos outros carros será de 1(tempo de espera para passagem de um carro);
      int tempo = 1;
      for(int i = inicio; i < saida; i++)
      {
        if(rotatoria->vias[via].veiculos[i].colisao)
        {
          acrescimoBatida(rotatoria);
        }
        if(rotatoria->vias[via].veiculos[i].tipo)
        {
          tempo = 2;
        }
      }
      rotatoria->vias[via].inicio = saida;
      if (x)
      {
        acrescimoTempo(rotatoria, tempo, 0, 0);
      } 
      if(rotatoria->vias[via].inicio >= rotatoria->vias[via].max)
      {
         rotatoria->vias[via].abilitada = 0;
      }
    }
  }
}

//Uma função reusável para preencher uma via de modo randômico;
void getVeiculos(Rotatoria* rotatoria, int via, int sem)
{
  int fim = rotatoria->vias[via].fim, inicio = rotatoria->vias[via].fim;
  int novos = getEntrada(), n;
  if(sem)
  {
    n = 1;
  }
  else
  {
    n = 5;
  }
  fim += novos;
  if(! rotatoria->vias[via].abilitada)
  {
    return;
  }
  if (fim >= rotatoria->vias[via].max)
  {
    rotatoria->vias[via].fim =  rotatoria->vias[via].max;
    return;
  }
  for(int i = inicio; i < fim; i++)
  {
    //iniciando os carros com um tempo de espera entre 1 e 2
    rotatoria->vias[via].veiculos[i].tempo = 1 + getPreferencialOuTipo();
    rotatoria->vias[via].veiculos[i].colisao = colisao(n);
    if(rotatoria->vias[via].quantOnibus > 0.2 * rotatoria->vias[via].max)
    {
      rotatoria->vias[via].veiculos[i].tipo = 0;
    }
    else if(rotatoria->vias[via].quantCarros >= 0.8 * rotatoria->vias[via].max)
    {
       rotatoria->vias[via].veiculos[i].tipo = 1;
    }
    else
    {
       rotatoria->vias[via].veiculos[i].tipo = getPreferencialOuTipo();
    }
    if(rotatoria->vias[via].veiculos[i].tipo)
    {
      rotatoria->vias[via].quantOnibus++;
    }
    else
    {
      rotatoria->vias[via].quantCarros++;
      if(rotatoria->vias[via].quantPreferenciais >= 0.01 * 0.8 * rotatoria->vias[via].max)
      {
        rotatoria->vias[via].veiculos[i].preferencial = 0;
      }
      else
      {
        rotatoria->vias[via].veiculos[i].preferencial = getPreferencialOuTipo();
        rotatoria->vias[via].quantPreferenciais++;
        if(rotatoria->vias[via].veiculos[i].preferencial)
        {
          rotatoria->vias[via].prefenciais ++;
        }
      }
    }
  }
  rotatoria->vias[via].fim = fim;
  return;
}

//imprime resultado
void imprimeResultado()
{
  float medias[4];
  for(int j = 0; j < 4; j++)
  {
      medias[j] = 0;
  }  
  for (int i = 0; i < numeroTestes; i++)
  {
    for(int j = 0; j < 4; j++)
    {
      medias[j] += tabela[i][j];
    }   
  }
  for(int j = 0; j < 4; j++)
  {
    medias[j] /= numeroTestes;
  }
  printf("Ao todo foram executados %d testes com o seguinte resultado(Média):\n", numeroTestes);
  printf("Tempo Normal : %.2lf\nTempo Preferencial : %.2lf\nTempo Semaforo : %.2lf\nTempo Pref. e Sem. : %.2lf\n", medias[0], medias[1], medias[2], medias[3]);
  printf("Destaca-se que os números são relativos e não possuem uma unidade de medida.\n");
}

//Inicializa todos os valores de uma instância de rotatória, posicionando os carros nas vias
void iniciaRotatoria(Rotatoria* rotatoria, int n)
{
  for(int i = 0; i < 3; i++)
  {
    rotatoria->vias[i].max = getMax();
    rotatoria->vias[i].inicio = 0;
    rotatoria->vias[i].fim = 0;
    rotatoria->vias[i].prefenciais = 0;
    rotatoria->vias[i].abilitada = 1;
    rotatoria->vias[i].quantCarros = 0;
    rotatoria->vias[i].quantPreferenciais = 0;
    rotatoria->vias[i].quantOnibus = 0;
    limpaRotatoria(rotatoria, i);
    getVeiculos(rotatoria, i, n);
  }
}

void limpaRotatoria(Rotatoria* rotatoria, int i)
{
  for(int j = 0; j < rotatoria->vias[i].max; j++)
  {
    rotatoria->vias[i].veiculos[j].tempo = 0;
  }
}

void simulaCasoAtual()
{
  Rotatoria rotatoria;
  iniciaRotatoria(&rotatoria, 0);
  int continua =  1, via;
  while(continua)
  {
    via = rand() % 3;
    getSaida(&rotatoria, via, 0, 0);
    via = rand() % 3;
    getVeiculos(&rotatoria, via, 0);
    continua = rotatoria.vias[0].abilitada + rotatoria.vias[1].abilitada + rotatoria.vias[2].abilitada;
  }
  tabela[indexTabela][0] = calculaTempo(&rotatoria);
  return;
}

void simulaCasoPreferencial()
{
  Rotatoria rotatoria;
  iniciaRotatoria(&rotatoria, 0);
  int continua =  1, via;
  while(continua)
  {
    via = rand() % 3;
    getSaida(&rotatoria, via, 0, 1);
    via = rand() % 3;
    getVeiculos(&rotatoria, via, 0);
    continua = rotatoria.vias[0].abilitada + rotatoria.vias[1].abilitada + rotatoria.vias[2].abilitada;
  }
  tabela[indexTabela][1] = calculaTempo(&rotatoria);
  return;
}

void simulaCasoSemaforo()
{
  Rotatoria rotatoria;
  iniciaRotatoria(&rotatoria, 1);
  int continua =  1, via = 0, viaAux;
  while(continua)
  {
    getSaida(&rotatoria, via, 1, 0);
    viaAux = rand() % 3;
    getVeiculos(&rotatoria, viaAux, 1);
    via++;
    via = via % 3;
    continua = rotatoria.vias[0].abilitada + rotatoria.vias[1].abilitada + rotatoria.vias[2].abilitada;
  }
  tabela[indexTabela][2] = calculaTempo(&rotatoria);
  return;
}

void simulaCasoPrefESema()
{
  Rotatoria rotatoria;
  iniciaRotatoria(&rotatoria, 1);
  int continua =  1, via = 0, viaAux;
  while(continua)
  {
    getSaida(&rotatoria, via, 1, 1);
    viaAux = rand() % 3;
    getVeiculos(&rotatoria, viaAux, 1);
    via++;
    via = via % 3;
    continua = rotatoria.vias[0].abilitada + rotatoria.vias[1].abilitada + rotatoria.vias[2].abilitada;
  }
  tabela[indexTabela][3] = calculaTempo(&rotatoria);
  return;
}

void simulaTransito()
{
  simulaCasoAtual();
  simulaCasoPreferencial();
  simulaCasoSemaforo();
  simulaCasoPrefESema();
}

//Todos os carros preferenciais deixam a rotatória e o tempo dos carros remanescentes aumenta;
void verificaPreferenciais(Rotatoria* rotatoria)
{
  for(int i = 0; i < 3; i++)
  {
    if(rotatoria->vias[i].prefenciais)
    {
      for (int j = 0; j < rotatoria->vias[i].prefenciais; j++)
      {
        for(int k = 0; k < 3; k++)
        {
          float aumento = ((rand() % 11) + 50) / 100;
          for(int l = rotatoria->vias[k].inicio; l < rotatoria->vias[k].fim; l++)
          {
            if(rotatoria->vias[k].veiculos[l].preferencial == 0)
            {
              rotatoria->vias[k].veiculos[l].tempo *= (1 + aumento);
            }
          }
        }
      }
      rotatoria->vias[i].prefenciais = 0;
    }  
  }
}
