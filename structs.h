#indef STRUCTS_H
#define STRUCTS_H
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

#endif
