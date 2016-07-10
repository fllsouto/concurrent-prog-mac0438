## Aula 12


### Problema dos filósofos famintos
```scala
sem garfo[5] = {1};
process filosofo[i=0..4] {
	while (true) {
    	//pensa
		p(garfo[i]);
        p(garfo[(i + 1) % 5]);
        
        //come
        v(garfo[i]);
        v(garfo[(i + 1) % 5]);
    }
}
        F/0
    g/0     g/1
  F/4         F/1
  g/4         g/2
    F/3     F/2
	    g/3


```

### Problema dos fumantes

```scala
F1 -> Tabaco  (0)
F2 -> Papel   (1)
F3 -> Fósforo (2)

sem ingr[3] = {0};
sem agent = 0;
process Agent {
	int i, j;
    while (true) {
    	ingredientes(&i, &j);
        if(i == 1 && j == 2)
        	v(sim[0])
        if(i == 0 && j == 2)
        	v(sim[1])
        if(i == 0 && j == 1)
        	v(sim[2])
        p(agente);
    }
}

process f1 {
	while (true) {
    	p(ingr[0]);
        //fuma
        v(agente);
    }
}

```

### Solução para o problema dos fumantes

```scala
//0 - Tabaco
//1 - Papel
//2 - Fósforo

sem a = 1, t = 0, p = 0, f = 0;
int tabacoNaMesa = 0, papelNaMesa = 0, fosforoNaMesa = 0;

process Agent {
  int ingr[2];
  while(true) {
    ingredientes(&ingr[0], &ingr[1])
	if(ingr[0] == 0 && ingr[i] == 1)
	  V(t);V(p);
	else if(ingr[0] == 0 && ingr[1] == 2)
	  V(t);V(p);
	else
	  V(p);V(f);

	p(a);
  }
}

process Fumante1 {
  while(true) {
    P(tabaco);
	//enrola cigarro
	V(a);
	//fuma
  }
}

process Fumante2 {
  while(true) {
    P(papel);
	//enrola
	V(a);
	//fuma
  }
}

process Entregador1 { //Verifica se existe tabaco na mesa
  while(true) {
  	P(t);
	P(e);
	if(papelNaMesa) {
	  papelNaMesa = 0
	  V(fosforo);
	} else if(fosforoNaMesa) {
	  fosforoNaMesa = 0;
	  V(papel);
	} else {
	  tabacoNaMesa = 1;
	}
	V(e);
  }
}
```