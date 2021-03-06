#include "output.h"

void init_output()
{
    for(int i = 2; i <= SIR; i++)
    {
        pinMode(i , OUTPUT);
        digitalWrite(i, OFF);
    }
    digitalWrite(SOLE, ON);
    digitalWrite(EN1,  ON);
    digitalWrite(EN2,  ON);
    digitalWrite(HEAT, OFF);
}

// 51 valor fixo

void write_init_output(String msg)
{
    int ar = ((msg[7]-'0')*10 + (msg[8]-'0'));
    int banda = ((msg[9]-'0')*10 + (msg[10]-'0')); 						
    // debug
    Serial.print("Temp AR: ");
    Serial.println(ar);
    ar = (51*ar)/10;
    analogWrite(ARC, ar);
    Serial.print("Banda morta: ");
    Serial.println(banda);
    
    // Resposta com o valor atual da entrada..
    Serial.print("Resposta do Escravo: ");
    Serial.println(msg);
}

void write_sol(String msg)
{
    int coil = ((msg[5]-'0')*10 + (msg[6]-'0')) + OFFSET_SOLE;

    digitalWrite(coil,  OFF);

    Serial.print("Resposta do Escravo: ");
    Serial.println(msg);
}

void write_output(String msg)
{
    //Escrever em uma saída          
    // string para inteiro aplicando offset, saida 1 no Arduino está mapeada para o pino 8, saída 2 para o pino 9 ....
    int coil = ((msg[5]-'0')*10 + (msg[6]-'0')) + OUTPUT_OFFSET;
    int value = (msg[7]-'0')*1000 + (msg[8]-'0')*100 + (msg[9]-'0')*10 +(msg[10]-'0');

    switch (coil)
    {
        case LAM_D:
            analogWrite(coil, value);
            break;
        case LAM_R:
            analogWrite(coil, value);
            break;
    }

    value = map(value, 0, 255, 0, 100);
    // int para string
    char buf[5];
    sprintf(buf,"%04d", value);

    // monta valor de retonro
    msg[7]=buf[0];
    msg[8]=buf[1];
    msg[9]=buf[2];
    msg[10]=buf[3];

    Serial.print("Resposta do Escravo: ");
    Serial.println(msg);
}

int EstadosMotores(int value, int sensor, int vento)
{
    int mot, hora[2] = {OUT1, OUT3}, anti[2] = {OUT2, OUT4};
    if (sensor == DINE)
        mot = 0;
    else 
        mot = 1;

    int lado = (analogRead(sensor));
    if(lado > 819)  lado = 819;
    else if(lado < 205)   lado = 205;
    lado = map(lado, 205, 819, 0, 100);

    /** 
    *! 4V = 818.4
    *! 1V = 204.6
    */
    if (vento > MAX_VENTO)
    {
        if(lado < 100)
        {
            digitalWrite(hora[mot],  ON);
            digitalWrite(anti[mot], OFF);
        }
        else
        {
            digitalWrite(hora[mot], OFF);
            digitalWrite(anti[mot], OFF);
        }
    }
    else
    {
        if (value > lado) 
        {
            digitalWrite(hora[mot],  ON);
            digitalWrite(anti[mot], OFF);
        }
            
        else if(value < lado)
        {
            digitalWrite(anti[mot],  ON);
            digitalWrite(hora[mot], OFF);
        }
        else
        {
            digitalWrite(hora[mot], OFF);
            digitalWrite(anti[mot], OFF);
        }
    }
    return lado;
}

void analog_write_output(String msg)
{
    // decodifica entrada a ser lida
    int aon = ((msg[5]-'0')*10 + (msg[6]-'0')) + ANALOG_OUTPUT_OFFSET;
    // de string para inteito
    int value = (msg[7]-'0')*1000 + (msg[8]-'0')*100 + (msg[9]-'0')*10 +(msg[10]-'0');
    
    // debug
    Serial.print("Escrita na Saida Analogica ");
    Serial.println(aon-ANALOG_OUTPUT_OFFSET);

    value = (51*value)/10;
    
    //escreve na saída
    analogWrite(aon, value);
    
    // Para esse caso, a resposta é um simples echo da mensagem original
    Serial.print("Resposta do Escravo: ");
    Serial.println(msg);
}