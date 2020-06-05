#include "serial.h"

void init_serial()
{
	Serial.begin(9600);
    Serial.println("Sistema Iniciado");
}

void msgHandler()
{
	if (Serial.available() > 0)
	{
		while(Serial.available() > 0 )
		{
			// ler até encontrar LF
			int tam;
			String msg, usar;
			msg = Serial.readStringUntil('\n');
			usar = msg;

			//teste
			Serial.print("Msg recebida: ");
			Serial.println(msg);
			
			//teste temperatura
			/*
			Serial.print("Temperatura: ");
			Le_temperatura();
			Serial.println(temperatura);*/
			
			while(msg[tam] != '\0')
				++tam;
			
			if(tam == 13) tam-=1;
			else if(tam == 14) tam-=2;
			
			//início válido
			if(msg[0] == ':')
			{
				Serial.println("Caracter de inicio (:) encontrado");
				//Cálculo LRC
				
				int j = 0, i;
				uint8_t dado[VAL], t, valor = 0;
				
				for (i = 1; i <= tam; i+=2)  //conversao para decimal   
				{                                  
					if (msg[i] > '9' && msg[i+1] > '9')
						dado[j] = ((usar[i]-'7'))*16 + ((usar[i+1]-'7'));
					
					else if (msg[i] > '9')
						dado[j] = ((usar[i]-'7'))*16 + (usar[i+1]-'0');
					
					else if (msg[i+1] > '9')
						dado[j] = (usar[i]-'0')*16 + ((usar[i+1]-'7'));
					
					else
						dado[j] = (usar[i]-'0')*16 + (usar[i+1]-'0');
						
					j++;
				}
				
				t = (uint8_t)(sizeof(dado)) - 1;
				valor = lrc(dado, t); //cálculo do lrc
				Serial.print("Valor LRC ");
				Serial.println(valor);

				Serial.print("Valor dado ");
				Serial.println(dado[t]);

				//compara se o LRC da msg é igual ao calculado
				/*
				if (valor == dado[t])
					Serial.println("LRC correto");
				else
				{
					Serial.println("LRC incorreto");
					break;
				}*/
				
				//ve para qual escravo é a mensagem
				if(msg[1]==SLAVE_ADR[0] && msg[2]==SLAVE_ADR[1] )
				{
					Serial.println("Endereço do escravo correto");
					
					//processa a mensagem
					int cmd = (msg[3]-'0')*10 + (msg[4]-'0');

					switch (cmd)
					{
						case READ_COIL:
						{
							read_output(msg);
							break;
						}
							
						case READ_INPUT:
						{
							read_input(msg);
							break;
						}
							
						case WRITE_COIL:
						{  
							write_output(msg);
							break;
						}
						case READ_ANALOG:
						{ 
							analog_read_input(msg);
							break;
						}

						case WRITE_ANALOG:
						{
							analog_read_output(msg);
							break;
						}
					}
				}
				else
					Serial.println("Mensagem para outro escravo");
			}
			else
				Serial.println("Comando desconhecido");
		}
	}
}

uint8_t lrc(uint8_t *dado, uint8_t t)
{
	uint8_t val_lrc = 0;
	for(int i = 0; i < t; i++)
	{
		if((val_lrc + dado[i]) & 0xFF)
		val_lrc = val_lrc + dado[i];
	}
	val_lrc = ((val_lrc^0xFF) + 1) & 0xFF;

	return val_lrc;
}