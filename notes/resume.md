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


#### Vantagens de usar Threads
- Executar multiplas atividades ao mesmo tempo
- Fácil criação e destruição comparado a processos (não ter que copiar toda área de compartilhamento deixa-as mais leve)
- Compartilhamento de dados
- Melhor desempenho
- Útil em sistema multiprocessados

#### Quando paralelizar?
Para paralelizar um algoritmo é necessário que hajam partes independentes nele, ou seja, o conjunto de escrita/leitura de cada parte do algoritmo tem que ser **disjunto** do conjunto de escrita/leitura da outra parte.

### Lidando com variáveis compartilhadas
Quantos estados possíveis o seguinte programa pode ter?
```
int x = 4
co x++;
// x = 1;
oc

  |--t1--> (t1-1)[eax <- x] -> (t1-2)[eax + 1] -> (t1-3)[x <- eax]
x=4
  |--t2--> (t2-1)[x <- 1]
  
//Existe 4 comandos distintos, podendo gerar os seguintes históricos de execução:
// A => (t2-1), (t1-1), (t1-2), (t1-3) x == 2
// B => (t1-1), (t2-1), (t1-2), (t1-3) x == 5
// C => (t1-1), (t1-2), (t2-1), (t1-3) x == 5
// A => (t1-1), (t1-2), (t1-3), (t2-1) x == 1

//Quatro diferentes históricos e 3 diferentes respostas
```

Cada comando é uma **ação atômica**, uma sequência indivisível de transições de estados. A granularidade das operações será baixa quando for sobre instruções de máquina, e alta quando for feita sobre diversas instruções de máquina(seção crítica).

A execução concorrente de vários processos serializa-se em diversos comandos executados pelo processador, um por vez. Essa serialização pode variar de uma execução para outra, fazendo com que esse fenômeno seja não determinístico.

#### Quais são as saídas para isso?
- Teste e depuração: Executar o programa para um número limitado de cenários e verificar o resultado.
- Análise exaustiva: Examinar todas as possíveis histórias de execução(n*m!/(m!)^n combinações, onde n é o número de processos e m as ações atômicas, sem contar os condicionais).
- Análie abstrata: Verificação formal usando lógica.

### Função da sincronização
A sincronização se faz útil pelos seguintes motivos:
- Restringir possíveis histórias indesejadas
- Preserva a dependência entre os processos

A sincronização pode ser obtida através da **exclusão mútua** ou da **sincronização por condição**

```
//Exemplo paralelizado de busca de valor máximo em um vetor.
//Esse programa é peculiar por criar n threads, uma para cada posição do vetor,
//todos terão que verificar a condição do if duas vezes e isso transforma
//a execução em uma grande fila serial.
int m = 0
co [i = 0 to n-1] {
  if (a[i] > m)
    <if (a[i] > m)
      m = a[i];>
}
write(m);
```

### Diferentes propriedades de um programa concorrente

- **Safety:** O programa nunca entra em um estado **ruim**
- **Liveness:** O programa eventualmente entra em um estado **bom**

Podemos entender como estado ruim dois programas estarem na mesma sessão crítica ao mesmo tempo, ou um deadlock. Enquanto o estado bom do liveness indica que cada histórico é finito e que uma hora o programa poderá entrar em sua sessão crítica.

###Corretudo
- **Parcial:** O programa termina
- **Total:** O programa termina e dá a resposta correta

###Referência Crítica
Referência a uma variável que é(ou pode) ser alterada por outro processo. Quando não houver referência crítica entre dois processos **a execução parecerá atômica**. Exemplo:
```
int x = 0, y = 0;
co {
  x = x + 1
  //
  y = y + 1
}
```
###Propriedade 'no máximo uma vez' (PNMUV)
Uma atribuição do tipo **x = e** satisfaz a **PNMUV** se:
- **e** contem no máximo 1 referência crífica e **x** não é lido por nenhum outro processo, ou
- **e** não tem referências críticas, e neste caso, **x** pode ser lido

### Granularidade Alta
Teremos granularidade alta se a propriedade **PNMUV** não acontecer e quisermos a execução atômica de vários comandos juntos e que sua sequência pareça indivisível. **Usamos a sincronização para construir atomicidade de alta granularidade.**