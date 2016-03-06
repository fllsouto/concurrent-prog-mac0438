# Resumo Programação Concorrente

### Conceituando os diferentes tipos de programação com processos

#### Concorrente
- Vários processos(threads) compartilham a CPU
- Modelo de memória compartilhada

#### Distribuída
- Processamento distribuído entre máquinas conectadas a uma rede comum
- Modelo de troca de mensagens

#### Paralela
- Processos concorrentes executam em processadores distintos
- Modelo de memória compartilhada ou troca de mensagens

Um processador com hyperthread continua tendo apenas um núcleo, porém o escalonamento das instruções pode ser feito a nível muito baixo, de instruções funcionais. Essa técnica é vantajosa e pode aumentar cerca de 10% a velocidade do processamento das instruções.

### O Problema da Sincronização

Existe basicamente duas formas de diferentes processos se sincronizarem: **exclusão mútua** e **sincronização por condição**. Exclusão mútua é o problema de garantir que sessões críticas do programa não rodem ao mesmo tempo por diferentes processos. Sincronização por condição é o problema de atrasar um processo até que uma dada condição seja verdadeira.

### Cache e Processadores

**Localidade temporal** - Variáveis que são constantemente acessadas precisam estar em um nível alto da hierarquia do cache para que o acesso seja mais rápido, exemplo: variantes de um loop e o conteudo do laço.

**Localidade espacial** - Trechos de código são copiados diretamente para o cache para que a execução sequencial das próximas instruições aconteceça de maneira otimizada, porém isso pode ser quebrado caso o trecho contenha um comando **JUMP**  para um outro segmento de código não mapeado

**Cache hit/miss** - Quando o programa precisa ler uma posição de memória ele irá primeira olhar no cache, caso o dado que ele precise esteja lá acontece um **cache hit**, caso contrário ocorre um **cache miss** e o dado e lido da memória principal e colocado tanto a disposição do programa como no cache. O processo de escrita de um novo valor é parecido, podendo ir do cache para a memória principal ou vice-versa.

É possível que aconteça uma inconsistência de cache entre dois processadores, exemplificando:
```
[p1]<--->[cache1]<-->|------------|
					 | Shared Mem |
[p2]<--->[cache2]<-->|------------|

Neste caso p1 e p2 tem uma cópia da variavel x nos seus respectivos caches. Se p1 alterar o valor de x para x' essa mudança será propagada para o seu cache e para a memória compartilhada, porém o cache de p2 não será atualizado, causando uma inconsistência.
```

Um falso compartilhamento de cache pode acontecer por que uma linha de cache pode conter diversas variaves, sendo que apenas uma delas pode ser de interesse do processador.

O modelo de memória distrubuída tenta minimizar esses problemas através da comunicação usando uma rede local e pela troca de mensagens.

### Processos e suas estruturas

```
Estrutura
[Segmento de Texto]
[Segmento de dados]
[		Pilha     ]
[Contador do Prog ]

Estados
New ----> Ready <-----> Running -----> Terminated
			^              |
		    |----Waiting<--|

PCB
[     CPU State    ]
[        PC        ]
[   Priority Num   ]
[Stack base & frame]
[     File Desc    ]

```


### Threads
Threads são processos leves que compartilham com seus pares um conjunto de dados. No Unix as threads são implementadas com o mesmo método de criação de processos, o **clone**, a diferenciação de uma e de outra é a área de compartilhamento, definida na passagem de parâmetros. Criar uma nova área de compartilhamento é algo muito custoso para o sistema operacional, por isso as threads são consideradas processos leves, elas não necessitam que toda área comum seja copiada. Até mesmo processos filhos criados com o fork dividem a mesma área com seus pais, e a medida que cada processo altera os dados a diferenciação vai acontecendo **on demmand**.

### Passos para paralelização

#### 1 - Decomposição
A computação a ser realizada é quebrada em unidades menores de trabalho(tasks)

#### 2 - Atribuição
As tarefas são atribuídas(escalonadas) para diferentes trabalhadores(processos)

#### 3 - Orquestração
A forma de como os trabalhadores vão cooperar entre si para executar a computação é definida

#### 4 - Mapeamento
Essa é uma etapa arbitrária na qual cada trabalhador é alocado em um posto de trabalho(processador), onde cada trabalhador pode alternar de posto de tempos em tempos

### Pseudo programando com threads

```
// process - criação de processos
// co-oc - criação de uma mais threads que esperam pelo seu termino
// wait - comando de sincronização

process iteracao[i=1 to n] {
	write(i);
}
comando
dos

//Execução possível
// 1 2 3 ... n comando
// 2 1 3 ... n comando
//...
// (n+1)! combinações

process iteracao {
	for[i=1 to n]
    	write(i);
}
comando;
do

//Execução possível
// 1 2 3 ... n comando
// 1 2 3 ... comando n
//...
// n+1 combinações

co [i=1 to n] {
	write(i);
}
comando;

//comando só é rodado quando todos terminarem, isso nos leva a n! combinações

co
	c1;
    c2;
  //c3;
	c4;
oc
c5;

// c1 e c2 são executados por uma thread e c3 e c4 por outra
// c5 só será executado quando ambas terminarem
```



