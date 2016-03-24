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
//Sem referência crítica, satisfaz o PNMUV
int x = 0, y = 0;
co {
  x = x + 1
  //
  y = y + 1
}

(x,y) = (1,1)

//Referência crítica em y, satisfaz o PNMUV
int x = 0, y = 0;
co {
  x = y + 1
  //
  y = y + 1
}

(x,y) = [(1,1),(2,1)]

//Referência crítica em y e x, não satisfaz a PNMUV por conter duas referências
//críticas e diferentes processos alterarem seus valores.
int x = 0, y = 0;
co {
  x = x + 1
  //
  y = x + 1
}
(x,y) = [(1,1),(2,1),(1,2)]
```
###Propriedade 'no máximo uma vez' (PNMUV)
Uma atribuição do tipo **x = e** satisfaz a **PNMUV** se:
- **e** contem no máximo 1 referência crítica e **x** não é lido por nenhum outro processo, ou
- **e** não tem referências críticas, e neste caso, **x** pode ser lido



### Granularidade Alta
Teremos granularidade alta se a propriedade **PNMUV** não acontecer e quisermos a execução atômica de vários comandos juntos e que sua sequência pareça indivisível. **Usamos a sincronização para construir atomicidade de alta granularidade.**


## Atômicidade e Sincronização
Tipos básicos como int, char e short são armazenados, lidos e escritos de forma atômica, seu tamanho é menor ou igual ao barramento de dados.

Quando temos alta granularidade entre os comandos processamos de sincronização para construir atômicidade.

```
Alta Granularidade + Sincronização => Atomicidade
```

### Await

Espera a condição B ser verdade, então executa S atômicamente. Essa espera pode ser um busy-waiting ou não. É garantido que **B será verdadeiro** para que S seja executado e **é garantido que S termine**.
```
<await (B) S;>
//Ex
<await (s > 0) s = s - 1>
//Podemos usar o await como uma barreira sem sessão crítica
<await (count > 0);>

<await (B)> === while(not B) //while usa busy-waiting
```

### Produtor-Consumidor sincronizado

```
int buf, p = 0, c = 0;

process Produtor {
  int a[MAX] = { ... };
  while (p < MAX) {
    <await (p == c);> //Buffer vazio
    buf = a[p];
    p = p + 1;
  }
}

process Consumidor {
  int b[MAX] = { ... };
  while (p < MAX) {
    <await (p > c);>
    b[p] = buf;
    c = c + 1;
  }
}

//Nesse código a cada item produzido o consumidor irá consumir, enquanto isso o produtor fica travado
// Ambas condições B respeitam o PNMUV pois são referências críticas mas seus próprios processos que alteram suas variáveis críticas e "não existe atribuição", apenas comparação. Com isso eu posso substituir:

<await (p > c);> == while (!(p > c))
<await (p == c);> == while (!(p == c))
```

## Sistema lógico formal
Um sistema lógico formal consiste em regras definidas em termos de:
- Um conjunto de simbolos
- Um conjunto de formulas construídas a partir dos símbolos
- Um conjunto de formulas que são admitidas como verdadeiras(axiomas)
- Regras de inferência, que especificam como derivar novas fórmulas de axiomas e outras formulas

```
H1, H2, ... Hn
--------------
	  C
//Se todas hipóteses H forem verdadeiras então C também será
```

### Segurança e Vitalidade

#### Propriedades de Segurança
- Uma propriedade P será garantida sse P for verdade em todos os históricos do programa concorrente
- O programa nunca entra em um estado "ruim", ou seja, um deadlock ou disputa por uma sessão crítica.

#### Propriedades de Vitalidade
- Uma propriedade P será garantida se P for verdade em algum dos históricos do programa concorrente
- O programa eventualmente entra em um estado "bom", ou seja, cada histórico e finito e termina

Exemplo

```
Segurança
- Exclusão mútua em regiões críticas (é ruim quando dois processos estão na região crítica ao mesmo tempo)
- Ausência de deadlock (sempre esperar por uma condição que nunca ocorre)

Vitalidade
- Termino do programa (cada histórico é finito)
- Todo processo consegue entrar na sua sessão crítica

Corretude parcial : O estado do programa é correto se o programa terminar (propriedade de Vitalidade)
Termino : Todo laço e chamada à procedimento termina (Propriedade de Segurança)

Corretudo total : Corretudo parcial + Termino (Termina e da a resposta correta)
```

#### Provando Segurança
Para provar segurança precisamos analisar os estados do programa, caso uma propriedade de segurança não seja satisfeita o programa deve entrar em um estado "ruim" que não satisfaz a propriedade. Um exemplo disso é uma falha na exclusão mutua, possibilitando que dois programas executem a mesma sessão crítica ou deadlock entre processos, no qual um deles espera infinitamente por um recurso.
Uma das formas de provar segurança é a **exclusão de configurações**:

```
co #processo 1
	;
    {pre(S1)} S1;

// #processo 2
	;
    {pre(S2)} S2;

//pre(Si) são predicados que tem valor verdade antes da execução de Si
//Se "pre(S1) ^ pre(S2) == falso" então os dois processos não irão executar concorrentemente, já que falso não caracteriza um estado do programa. Esse "estado" seria um estado ruim
//Pegando como exemplo o produtor-consumidor:
//(p > c) ^ (p == c) esse estado é falso para qualquer configuração, portanto o
//programa é seguro
```

### Vitalidade e Justiça
A propriedade de vitalidade está ligada com o conceito de justiça. Ter justiça em um escalonamento é garantir que um processo tenha chance de progredir na sua execução, independente das ações de outro processos.
- **Ação atômica elegível**: é a próxima ação atômica que um processo poderá executar
- **Ação atômica incondicional**: é próxima ação atômica que não possui uma condição de atraso

```
bool continue = true;
co
	while (continue); //p1
//
	continue = false; //q1
oc
```

Três casos:
- 1 processador e escalonamento não-preemptivo. Resulta em não término do programa se p1 rodar primeiro, portanto não é bom e não tem vitalidade
- 1 processador e escalonador Round-Robin. Resulta que p1 e q1 terão uma fatia da execução, garantindo que ambos terminem e que a propriedade de vitalidade seja verdadeira
- 2 processadores alocados a ambos processos. Resulta no término e na vitalidade

Uma política de escalonamento segue **justiça incondicional** se cada ação atômica incondicional elegível é executada em algum momento. O RR e execução paralela são desse tipo.

Uma política de escalonamento segue **justiça fraca** se:
- Segue justiça incondicional
- Dado que tenho uma ação atômica para ser executada ```<await (B) S;>``` e que  **p** é o processo que espera por **B**, se **B** se tornar verdade ela deve se manter até que **p** seja escalonado e possa executar sua ação atômica até o final sem sofrer preempção.

Pensando assim round-robing não necessariamente respeita isso, pois **B** pode se tornar verdade no final do time-slice de **p**, sua condição será verdade mas o escalonador irá interromper que a sessão crítica de **p** execute.

Uma política de escalonamento segue **justiça forte** se:
- Segue justiça incondicional
- Toda ação atômica condicional elegível é eventualmente executada, dado que a condição é verdade um número finito de vezes em cada histórico de execução, ou seja se uma condição se tornar verdade e um processo espera por ela este com certeza irá executar. 
Exemplo:
```
bool continue = true, try = false;
co
	while (continue) {try = true; try = false;} //p1
	//<await (try) continue = false;> //q1
oc
```

Na **justiça forte** o programa eventualmente terminará pois em alguma execução q1 conseguirá executar pois _try_ é verdadeiro. Enquanto que na **justiça fraca** q1 pode esperar infinitamente por _try_ verdadeiro, levando o programa a não terminar.