# Programacao-Concorrente
Repositório criado para colocar os códigos criados para a disciplina programação concorrente

## Laboratório 4:
Comparação dos tempos para implementação com lock, rwlock e minha implementação (Para 10 milhões de tarefas):

### 1 thread:
Lock:                   0.9s
Rwlock:                 1.5s
Minha implementação:    1.2s

### 2 threads:
Lock:                   1.8s
Rwlock:                 3.3s
Minha implementação:    2.7s

### 4 threads:
Lock:                   1.6s
Rwlock:                 4s
Minha implementação:    4.3s

Sendo assim, percebo uma diferença de tempo incomum e que não deveria estar acontecendo (Talvez por alguma questão da minha máquina).
Os tempos das implementações com minhas funções e do phtreads ficou semelhante, como era de se esperar, porém, o tempo de execução para o
lock apenas não deveria ter ficado tão baixo em comparação aos outros
