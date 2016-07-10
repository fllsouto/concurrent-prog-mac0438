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
  x = y + 1
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

### O Problema da seção crítica
N processos executam uma seção de código crítica e uma seção não crítica. Um processo que obtem acesso à sessão crítica, cumprirá as suas atividades então sairá. Uma solução de seção crítica precisa respeitar 4 propriedades:
- Exclusão mútua
- Ausência de deadlock (livelock)
- Ausência de espera desnecessária
- Entrada garantida

Os três primeiros são propriedades de segurança e o ultimo de vitalidade.

Um artifício útil para travas é o uso de um spin lock:
```
while(true){
	<await (!lock) lock = true;>
	seção crítica
	lock = false;
	seção não crítica
}
```

É possível utilizar apenas uma varíavel para travar n processos. A maioria dos processadores modernos implementa em seu hardware algo conhecido com **TS(test-and-set)**

```
<await (lock == false) lock = true;>

bool TS(bool lock) {
	<
    bool initial = lock
    lock = true;
    return initial;
    >
}

lock = false
TS(lock) => retorna false e lock == true
lock = false
```
O problema de utilizar **Spin-Locks** é que precisamos de um escalonador que pratique a **Justiça forte**, o que é impraticável. Para contornar isso precisamos de um escalonador de exerça justiça fraca e não seja prejudicial a todos os processos. Além disso não temos um controle fino de quem entra na seção crítica, o mesmo processo pode executa-la repetidas vezes.

### Implementando o await()
Qualquer solução para a seção crítica pode ser utilizado para implementar a ação atômica incondicional **<S;>**.
```
<await (B) S;
Protocolo de entrada
while (!B) {
	Protocolo de saída
    Protocolo de entrada
}
S;
Protocolo de saída
```

TODO: Colocar as soluções daqueles exercícios de SC com instruições diferentes

## Algoritmos Classicos pra SC

### Bakery

```
//proc = 4
//senha = [3,1,2]
<senha[4] = max([3,1,2]) + 1>
for(j=1 to 4 st j!=1)
	<await (senha[j] == 0 or senha[4] < senha[j];)>
```

## Semaforos
Em muitos problemas os processos realizam espera ocupada, que consiste em um processo ficar verificando uma condição até que ela seja verdade. Do jeito que implementamos com spin lock não é muito performático, pois são complexos, ineficientes e precisam de instruções especiais como a **fetch-and-add** e a **test-and-add**. Se o número de processos é menor ou igual ao número de processadores pode ser que não exista degradação da performance, mas sabemos que esse não é o cenário usual, é muito mais comum termos N processos e P processadores, onde costuma ser N > P, portanto um processador rodando um processo em espera ocupada é um desperdício de tempo, seria melhor que o processo fosse colocado para dormir e outro processo fosse colocado para ser executado.
Adotaremos a solução dos semáforos, criado por Dijkstra em 1968.

### Operações
As operações de um semáforo são:

- P(sem) é usada para atrasar um processo até que um evento ocorra, ou seja, espera até que seu valor seja positivo então decrementa o valor.
- V(sem) é usada para sinalizar a ocorrência de um evento, incrementando o valor do semáforo.

sem é uma variável inteira não negativa compartilhada entre os processos. Existe um paralelo entre as operações e o comando <await (B) S;>

```bash
P(s): <await (S > 0) s = s - 1;>
v(s): <s = s + 1;>

## Uma possível implementação:

typedef struct {
	int value;
  struct _process *l; # FIFO Queue
}

block();   # Suspende o processo que invocou a op
wakeup(P); # Retorna a execução de um processo bloqueado P

P(s):
if (S.value)
	S.value--;
else {
	adiciona este processo em S.L;
  block();
}

V(s):
if (S.L está vázia)
	S.value++;
else {
	remove um processo R de S.L;
  wakeup(R);
}

P0, P1, P2
sem lock = 1;

t0 - P0 ---> P(lock)[S.value == 1 -> S.value-- -> S.value == 0]
t1 - P1 ---> P(lock)[S.value == 0 -> S.L {P1} ]
t2 - P2 ---> P(lock)[S.value == 0 -> S.L {P1, P2} ]
t3 - P0 ---> V(lock)[S.L not empty -> S.L pop {P1, P2} -> wakeup(P1) -> S.L {P2}]
t4 - P1 ---> V(lock)[S.L not empty -> S.L pop {P2} -> wakeup(P2) -> S.L {}]
t5 - P5 ---> V(lock)[S.L is empty -> S.value++ -> S.value == 1]
```

Os semáforos provém diretamente:
- Exclusão mútua
- Condição de sincronzação

Ex:

```bash
sem mutex = 1;
process CS [i = 0 to n] {
	while(true) {
  	P(mutex);
    SC
    V(mutex);
    SNC
  }
}

# Processo P quer executar apenas depois que o processo Q terminar

sem lock = 0;

t0 - Q Tarefa // P ---> P(lock)[S.value == 0 -> S.L {P} ]
t1 - Q ---> V(lock)[S.L not empty -> S.L pop {P} -> wakeup(P) -> S.L {}]
t2 - P Tarefa // Q fim
```

Com essa ideia o V passa a ser útil para sinalizar um evento e P para esperar um evento.

### Produtores e consumidores
Eles interagem através d eum unico buffer compartilhado que tem as seguintes propriedades:
- Está inicialmente vazio
- P espera até que o buffer esteja vazio
- C aguard até que o buffer esteja cheio

Usando semáforos teremos:
- Uso de mais de um semáforo nos eventos
- Esses semáforos são utilizados para sinalizar:
	- Quando processos alcançarem pontos criticos de execução
	- Mudança de variáveis compartilhadas
- Essa solução considera o estado do buffer

```bash
typeT buf;

process Producer[i = 1 to M] {
	while(true) {
  	...
    # Produz dados e armazena no buffer
    P(empty)
    buf = data
    V(full)
  }
}

process Consumer[i = 1 to N] {
	while(true) {
  	...
    # Produz dados e armazena no buffer
    P(full)
    result = buf
    V(empty)
  }
}
```

Esses semáforos são utilizados para marcar a mudança de estado e recebem o nome de **semáforos binário divididos**. Porém essa solução não previne que tenhamos uma rajada de produtores, que só num futuro incerto começará a ser consumida. Podemos ter uma fila circular de mensagem não consumidas e dois ponteiros.
O ponteiro **front** aponta para a primeira mensagem da lista e o ponteiro **rear** para a primeira posição livre da lista.

As operações serão:
```bash
# buf[rear] = data;
# rear = (read + 1) % n;

# result = buf[front];
# front = (front + 1) % n;

# Solução 1x1

typeT buf;
int front = 0, rear = 0;
sem empty = 0, full = 0;

process Producer {
	while(true) {
  	...
    # Produz dados e armazena no buffer
    P(empty)
    buf[rear] = data;
    rear = (read + 1) % n;
    V(full)
  }
}

process Consumer {
	while(true) {
  	...
    # Produz dados e armazena no buffer
    P(full)
		result = buf[front];
		front = (front + 1) % n;
    V(empty)
  }
}

```
### Semáforos genéricos
Servem como contadores de recurso:
- Iniciam com valo inteiro
- São apenas não negativos
- Usados para sincronização

Na solução para MxN produtores e consumidores precisamos de 4 semáforos diferentes.

```bash
# buf[rear] = data;
# rear = (read + 1) % n;

# result = buf[front];
# front = (front + 1) % n;

# Solução MxN

typeT buf;
int front = 0, rear = 0;
sem empty = n, full = 0;
sem mutexD = 1, mutexF = 2;

process Producer[i = 1 to M] {
	while(true) {
  	...
    # Produz dados e armazena no buffer
    P(empty)
    P(mutexD)
    buf[rear] = data;
    rear = (read + 1) % n;
    V(mutexD)
    V(full)
  }
}

process Consumer[i = 1 to N] {
	while(true) {
  	...
    # Produz dados e armazena no buffer
    P(full)
    P(mutexF)
		result = buf[front];
		front = (front + 1) % n;
    V(mutexF)
    V(empty)
  }
}

# Nesse algoritmo os produtores vão passar pelo comando P(empty), pois empty == n Em seguida todos ficarão presos em P(mutexD), apenas 1 deles irá passar e produzir um dados, e só após produzir ao menos 1 dado que os consumidores serão acordados para consumir, ao fazer V(full)

```

### Problema dos fílósofos famintos
O problema consiste em **N** filósofos dispostos em uma mesa circular, onde cada um tem um prato de macarronada na sua frente. Cada filósofo precisa fazer um uso de dois garfos para comer, um para segurar o macarrão e outro para enrolar. Existe um garfo entre cada filósofo, portanto existe **K** garfos, onde **K == N**. Os objetivos do problema são:

- Evitar que os filósofos morram de fome, esperando que seu vizinho solte o garfo.
- Esse problema é uma abstração para o compartilhamento de recursos entre **N** processos.
- Dois filósofos vizinhos não podem comer ao mesmo tempo
- floor(N/2) é a quantidade de filósofos que irão comer

Solução inicial:

```bash
process Philosopher [i = 0 to 4] {
	while(true) {
  	pensa;
    pega os garfos;
    come;
    libera os garfos;
  }
}

sem fork[5] = {1, 1, 1, 1, 1}
process Philosopher [i = 0 to 4] {
	while(true) {
  	P(fork[i]);P(fork[(i + 1) % N])
    come;
    V(fork[i]);V(fork[(i + 1) % N])
  }
}

```

### Mutex e Semáforos: Como diferenciar
Tanto mutex quanto semáforos são mecanismos de sincronização entre processos, seja para acessar um recurso compartilhado ou seja para sinalizar o termino de uma ação. A melhor forma de diferenciar os dois é pensar que o **Mutex** é um **mecanismo de trava**, usado para sincronizar o acesso a um recurso, apenas uma thread ou processo pode adquirir um mutex por vez e opera-lo, isso significa que apenas o detentor do mutex pode libera-lo. Por sua vez o **Semáforo** é um **mecanismo de sinalização**, onde um processo pode comunicar a outro que terminou seu acesso a um recurso compartilhado ou o cálculo de alguma computação. Neste caso um processo P1 pode liberar um semáforo que outro processo P2 está esperando, internamente a ideia é a de envio de mensagens entre um processo e outro ([ref](http://www.geeksforgeeks.org/mutex-vs-semaphore/)).

### O problema dos leitores e descritores
Carateristicas do problema:
- Dois tipos de processos compartilham uma base de dados
- Leitores só examinam os registros
- Escritores examinam a base e a alteram
- Um escritor precisa de acesso exclusivo à base de dados
- Se nenhum escritor estiver acessando a base de dados, pode haver qualquer quantidade de leitores concorrentemente lendo a base
- Leitores tem que esperar até que não exista nenhum escritor na base
- Escritores tem que esperar até que não exista nenhum leitor ou escritor na base

Primeira solução:
```bash
int nr = 0; # Quantidade de leitores
sem rw = 1

process Reader[i = 1 to M] {
	while(true) {
  	...
    <
    nr = nr+1;
    if(nr == 1) P(rw);
    >

    le a base;

    <
    nr = nr+1;
    if(nr == 1) P(rw);
    >
  }
}

process Reader[j = 1 to N] {
	while(true) {
  	...
    P(rw)
    escreve na base;
    v(rw);
  }
}
```
Temos que delimitar as regras do algoritmo:

- Evitar que (nr > 0 && nw > 0) || nw > 1
- Garantir que (nr == 0 || nw == 0) && nw <= 1

```bash
# Leitor
< await(nw == 0) nr = nr + 1; >
le a base;
< nr = nr - 1; >

# Escritor
< await(nr == 0 && nw ==0) nw = nw + 1; >
escreve na base;
<nw = nw - 1;>
```

É difícil substituir um comando await por um semáforo, neste caso temos que utilizar a técnica conhecida como **Passagem de bastão**. Iremos usar semáforos divididos para isso:

```bash
int nr = 0; # Quantidade de leitores
int nw = 0; # Quantidade de escritores
sem e = 1, # Semáforo que controla a entrada em cada ação atômica
    r = 0, # Semáforo associado com a condição do leitor
    w = 0; # Semáforo associado com a condição do escritor
int dr = 0, # número de leitores esperando a condição ser verdadeira
    dw = 0; # número de escritores esperando a condição ser verdadeira

process Reader[i = 1 to M] {
	while(true) {
  	...
    P(e);
    if(nw > 0) { dr = dr + 1; V(e); P(r); }
    # if(nw > 0 or dw > 0) { dr = dr + 1; V(e); P(r); }
    # Com o treço acima caso exista um escritor em operação que acabou de escrever e outro esperando o leitor cederá sua vez para o escritor que espera
    nr = nr + 1;
    SIGNAL
    le a base;
    P(e);
    nr = nr - 1;
    SIGNAL;
  }
}

process Writer[j = 1 to N] {
	while(true) {
  	...
    P(e);
    if(nr > 0 or nw > 0) { dw = dw + 1; V(e); P(w); }
    nw = nw + 1;
    SIGNAL
    le a base;
    P(e);
    nw = nw - 1;
    SIGNAL;
  }}
  
# SIGNAL:
if(nw == 0 and dr > 0) { # Priorizando Leitores
  dr = dr - 1; V(r); # Acorda um leitor, ou
} elseif(nr == 0 and nw == 0 and dw > 0) {
  dw = dw - 1; V(w); # Acorda um escritor, ou
} else
  V(e); # Libera o lock de entrada

# if(dw > 0) { # Priorizando Leitores
#   dw = dw - 1; V(w); # Acorda um leitor, ou
# } elseif(dr > 0) {
#   dr = dr - 1; V(r); # Acorda um escritor, ou
# } else
#   V(e); # Libera o lock de entrada

#Alterando o signal para que a prioridade seja dos escritores
```

Esse algoritmo não é totalmente justo, uma melhor abordagem seria:
- Envie um novo leitor para o final da fila se houver um escritor esperando (A);
- Envie um novo escritor para o final da fila se houver um leitor esperando (B);
- Acorde um escritor que estiver esperando (se houver) quando um leitor terminar de ler (C);
- Acorde todos os leitores que estiverem esperando (se houver) quando um escritor terminar de escrever; caso contrário, acorde um escritor que estiver esperando (se houver) (D);

```bash
if(nw > 0 or dw > 0 ){ dr = dr + 1; V(e); P(r); } # (A)

if(nr > 0 or dr > 0 or nw > 0){ dw = dw + 1; V(e); P(w); } # (B)

# SIGNAL Leitor:
# (C)
if(nr == 0 and dw > 0) {
  dw = dw - 1; V(w); # Acorda um escritor
} else
  V(e); # Libera o lock de entrada

# SIGNAL Escritor:
# (D)
if(dr > 0) {
  dr = dr - 1; V(r); # Acorda um leitor, ou
} elseif(dw > 0) {
  dw = dw - 1; V(w); # Acorda um escritor, ou
} else
  V(e); # Libera o lock de entrada
```

### Alocação de recursos
Consiste no problema de decidir quando um processo pode ter acesso a um recurso(entrar na SC, acesso a uma base de dados, etc...). Vimos casos específicos, teria esse problema uma formulação mais geral? Podemos modelas esse problema através de duas operações:

- **Request**: O processo informa quantas unidades de recurso compartilhado quer acessar e seu identificador.
- **Request**: O processo libera os recursos utilizados para outros processos.

```bash
request(parametros):
  <
  await(request poder ser atendido)
  acessa as unidades
  >

release(parametros)
  <
  retorne as unidades;
  >

# Solução com semáforos e passagem de bastão

request(parametros):
  P(e);
  if(request poder ser atendido) DELAY;
  acessa as unidades;
  SIGNAL;

release(parametros)
  P(e);
  retorne as unidades;
  SIGNAL;

request(parametros):
  P(e);
  if(!free) {
    insere(tempo, id) na fila; # Fila ordenada por tempo (shortes job first)
    V(e);
    P(b[id]);
  }
  free = false;
  V(e);

release(parametros)
  P(e);
  free = true;
  if(fila não está vazia) {
    remova primeiro par (tempo, id) da fila;
    V(b[id]);
  }else
    V(e);
```

### Vantagens e Problemas com Semáforos
Semáforos facilitam a implementação de exclusão mútua, a sinalização entre processos e a implementação do comando **await**, entretanto criar semáforos ou atrasar a execução de threads pode degradar a performance do programa severamente. Além disso o semáforo é um mecanismo de baixo nível, pode ser difícil obter uma abstração de alta nível utilizando ele, como também pode dificultar a depuração e entendimento do programa.

Uma alterativa para os semáforos são os monitores, eles visam mitigar as desvantagens citadas criando um mecanismo abstrato similar a uma classe.

Monitores encapsulam a representação de um objeto abstrato, tendo internamente estados e operações. Essa representação só ser operada ou alterada através da sua "interface pública".

#### Vantagens

- Processos sempre vão chamar procedimentos dos monitores, e internamente podem tratar da exclusão mutua
- Os processos sempre tem que verificar as variáves dos monitores, as quais são variáveis de condição similares a semáforos

O monitor será modelado para representar um recurso compartilhado pelos processos.

```bash
monitor nomedomonitor {
	variaveis permanentes;
  comandos de inicialização;
  demais procedimentos;
}
```

Usando monitores existem uma serialização nas **chamadas** que os processos podem fazer, duas chamadas para o mesmo procedimento não podem estar ativas ao mesmo tempo, isso vale para chamadas para diferentes procedimentos (estranho isso). Se parece com o modelo de atores, cada chamada ao monitor será uma mensagem, e essas mensagens serão processadas uma por vez e sequencialmente. Por ser assim se um processo pede ao monitor um recurso que está em uso este processo não ficará esperando. Se ficasse isso poderia causar um deadlock, portanto o monitor bloqueia o processo até que sua condição seja verdadeira e processa pedidos seguintes.

A liberação dos recursos tem que partir dos processos que usam o monitor, por que nesse próprio método o monitor vai permitir que processos que estavam dormindo retomem a sua execução. Neste ponto existe o seguinte problema que precisamos resolver:

Considere uma variavel compartilhada SIGNAL do monitor M e os processos P1 e P2. P1 requisitou a variável SIGNAL de M e está em execução. Durante esse tempo P2 também requisita SIGNAL e entra na fila de espera. Ao terminar P1 vai sinalizar que M está livre, o que deve acontecer? Exitem duas abordagens:

- **Sinaliza e continua**: P1 irá sinalizar, P2 fica na fila de espera de execução de M e P1 termina sua execução. Ele sai da fila da variável SIGNAL e vai para a fila do M. Essa abordagem é não-preemptiva.
- **Sinaliza e espera**: P1 sinaliza e cede sua execução para P2. P1 irá retornar sua execução em algum momento do futuro. Essa abordagem é preemptiva.



```bash
  Início
  |
  | Chamada a algum procedimento do Monitor
  |
  V
  Fila de entrada <-|<-------|
  |                 |        |
  | Monitor         |        |
  | livre           | S & E  |
  V                 |        |
  Executando o ---->|        |
  monitor        <--|        |
  |                 |        |
  | Espere          |        |
  |                 | S & E  | S & C
  V                 |        |
  Fila da variável->|        |
  de condição   ------------>|

```

Slide 33