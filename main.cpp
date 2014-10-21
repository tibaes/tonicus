#include "flow.hpp"

int main(int argc, char **argv)
{
  /* Tamanho do ambiente monitorado em (x,y)
   * cuidado com a unidade (metros, km, absoluto)
   */
  Point2f realSize(1.0,1.0);
  
  /* frames por segundo */
  float fps = 30;
  
  /* Velocidade limite para definir a direcao do objeto (ou se parado).
   * Note que a velocidade sera entregue de acordo com a medida de espaco
   * de realSize, entao se for definido em metros, a velocidade se da em m/s
   * Neste exemplo, realSize foi colocado em 1.0, logo a velocidade eh
   * relativa a propria imagem.
   */
  Point2f velocidadeLimite(0.06,0.06);
  
  OpticalFlow of(realSize,fps,velocidadeLimite,argv[1],argv[2]);
  
  char dirText[128];
  switch (of.getDirectionX()) {
    case -1:
      sprintf(dirText,"Esquerda");
      break;
    case 1:
      sprintf(dirText,"Direita");
      break;
    case 0:
      sprintf(dirText,"Parado");
      break;
  }
  printf("Direcao: %s, velocidade: %f\n",dirText,of.getSpeedX());
  
  of.draw();
  
  return 0;
}