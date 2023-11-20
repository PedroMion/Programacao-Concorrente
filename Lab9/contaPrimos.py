from multiprocessing.pool import Pool
import math
import random

# Função passada no lab, adaptada para python
def ehPrimo(n):
   if (n<=1):
       return 0
   if (n==2):
       return 1
   if (n%2==0):
       return 0
   for i in range(3, int(math.sqrt(n)) + 1, 2):
       if(n%i==0):
           return 0
   return 1


if __name__ == '__main__':
   # Criando o pool e sorteando um valor, para aumentar o range, basta aumentar o segundo número do randint
   pool = Pool()
   rand = random.randint(1, 1000)

   # Criando lista de 1 até o número sorteado
   numeros = list(range(rand+1))

   # Executando a função para cada valor de numeros, usando o pool
   resultado = pool.map(ehPrimo, numeros)
  
   print(f'Quantidade de primos entre 1 e {rand}: {resultado.count(1)}')
