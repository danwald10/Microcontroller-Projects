#include <XMC1100.h>

void wait();
void leuchten();

int h1=0;
int h2=0;
int h3=0;
int h4=0;
int h5=0;
int setAB=0;

int main(void)
{


	//Ein-/Ausgänge
	PORT0->IOCR0=0x0;
	PORT0->IOCR4=0x808000;
	PORT0->IOCR8=0x8080;
	PORT0->IOCR12=0x0;
	PORT1->IOCR0=0x80808080;
	PORT1->IOCR4=0x80;
	PORT2->IOCR0=0x0;
	PORT2->IOCR4=0x0;
	PORT2->IOCR8=0x0;

	//Enable Interrupts
	NVIC->ISER[0]=0x1E00078;

	//Digital Input Enable
	PORT2->PDISC=0x0;

	//ERU0 config---------------------
	ERU0->EXISEL=0x1111;

	ERU0->EXICON[0]=0x205;
	ERU0->EXICON[1]=0x215;
	ERU0->EXICON[2]=0x225;
	ERU0->EXICON[3]=0x235;

	ERU0->EXOCON[0]=0x10;
	ERU0->EXOCON[1]=0x10;
	ERU0->EXOCON[2]=0x10;
	ERU0->EXOCON[3]=0x10;
	//---------------------------------


	// Clock configuration
		SCU_GENERAL->PASSWD = 0x000000C0UL; 					// disable bit protection
		SCU_CLK->CLKCR      = 0x3FF00100UL; 					// MCLK = 32MHz, PCLK = 32MHz
		while((SCU_CLK->CLKCR & SCU_CLK_CLKCR_VDDC2LOW_Msk));   // wait until clock ready
		SCU_GENERAL->PASSWD = 0x000000C3UL; 					// enable bit protection



		// ---------------------- CCU4 Timer slice 0  ----------------------

		/* Step 1 */
		SCU_GENERAL->PASSWD = 0x000000C0UL;				// disable bit protection
		SCU_CLK->CGATCLR0  |= 0x01 << 2;				// activate CCU4 clock
		SCU_GENERAL->PASSWD = 0x000000C3UL;				// enable bit protection

		/* Step 2 */
		CCU40->GIDLC |= 0x0F;							// enable prescaler block

		/* Step 3 */
		CCU40->GCTRL = 0x00000000;                      // use Module Clock (32 MHz)

		/* Step 4 */
		CCU40_CC40->INS=0x410002;
		CCU40_CC42->INS=0x410002;

		CCU40_CC40->SRS=0x00;							//going to SR0
		CCU40_CC42->SRS=0x200;							//going to SR2

		CCU40_CC40->INTE=0x100;							//Event0 enable

		CCU40_CC42->INTE=0x100;							//Event0 enable

		CCU40_CC41->INTE =0x1;							//Period match
		CCU40_CC41->TC  = 0x0;                          // edge aligned mode
		CCU40_CC41->PSC = 0x0A;							// prescaler for Timer (2048)
		CCU40_CC41->PRS = 1562;							// period value = 1s

		//Shadow transfer
		CCU40->GCSS |= 0x1111;							// activate shadow transfer

		/* step 5 */
		//Not Used

		/* Step 6 */
		CCU40->GIDLC |= 0x01 ;							// enable Timer slice 0

		// -------------------------------------------------------------------



		CCU40_CC40->TCSET = 0x1;
		CCU40_CC41->TCSET = 0x1;
		CCU40_CC42->TCSET = 0x1;
		CCU40_CC43->TCSET = 0x1;

		while(1)
		{
		}

		return 0;
	}

	// Timer slice 0 period match Interrupt
	void IRQ_Hdlr_21(void)
	{
		if((PORT0->IN & 0x01) == 0x01){
				//LB3=1
				if(h3&&((PORT1->IN & 0x01) == 0x01)){
					//h3=1 and Soll_Down=1

					h3=0;
					PORT1->OMR=0x10000; //Soll_Down=0
					wait();

					if(h2||h1){
						PORT1->OMR=0x01; //Soll_Down=1
						leuchten();
					}
					else if(h4||h5){
						PORT1->OMR=0x02; //Soll_Up=1
						leuchten();
					}
					else
						leuchten();
					}
					else if(h3&&((PORT1->IN & 0x02) == 0x04)){
							//h3=1 and Soll_Up=1

							h3=0;
							PORT1->OMR=20000; //Soll_Up=0
							wait();

							if(h4||h5){
							PORT1->OMR=0x02; //Soll_Up=1
							leuchten();

							}
							else if(h1||h2){
								PORT1->OMR=0x01; //Soll_Down=1
								leuchten();
								}
							else
								leuchten();
					}
					else
						leuchten();
				}

	}

	void IRQ_Hdlr_22(void)
		{
			PORT0->OMR=0x20;
			PORT0->OMR=0x40;
			PORT0->OMR=0x20<<16;
			PORT0->OMR=0x40<<16;
			PORT0->OMR=0x20<<16;
		}
	void IRQ_Hdlr_23(void)
		{
			if((PORT0->IN & 0x04) == 0x04){
					//LB1=1
					if(h1){
						//h1=1

						h1=0;
						PORT1->OMR=0x10000; //Soll_Down=0
						wait();

						if(h2||h3||h4||h5){
							PORT1->OMR=0x02; //Soll_Up=1
							leuchten();
						}
						else
							leuchten();
					}
				}

		}
	void IRQ_Hdlr_3(void)
	{
		if((PORT2->IN & 0x01) == 0x01){
			//LB2=1
			if(h2&&((PORT1->IN & 0x01) == 0x01)){
				//h2=1 and Soll_Down=1

				h2=0;
				PORT1->OMR=0x10000; //Soll_Down=0
				wait();

				if(h1){
					PORT1->OMR=0x01; //Soll_Down=1
					leuchten();
				}
				else if(h3||h4||h5){
					PORT1->OMR=0x02; //Soll_Up=1
					leuchten();

				}
				else
					leuchten();
			}

			else if(h2&&((PORT1->IN & 0x02) == 0x04)){
					//h4=1 and Soll_Up=1

					h2=0;
					PORT1->OMR=20000; //Soll_Up=0
					wait();

				if(h3||h4||h5){
					PORT1->OMR=0x02; //Soll_Up=1
					leuchten();

					}
					else if(h1){
						PORT1->OMR=0x01; //Soll_Down=1
						leuchten();
						}
					else
						leuchten();
			}
				else
					leuchten();

	}

		else if((PORT2->IN & 0x10) == 0x10){
				//LB5=1
			if(h5){
				h5=0;
				PORT1->OMR=0x20000; //Soll_Up=0
				wait();

				if(h1||h2||h3||h4){
					PORT1->OMR=0x01; //Soll_Down=1
					leuchten();
				}
				else
					leuchten();
			}
		}
	}
	void IRQ_Hdlr_4(void)
	{
			if((PORT2->IN & 0x20) == 0x20){
				//Button3
				h3=1;
				if(((PORT1->IN & 0x01) == 0x00)&&((PORT1->IN & 0x02) == 0x00)){
					//Soll_Down=0 und Soll_Up=0
						if(h4||h5){
							PORT1->OMR=0x02; //Soll_Up=1
							leuchten();
						}
						else if(h1||h2){
							PORT1->OMR=0x01; //Soll_Down=1
							leuchten();
						}
						else
							leuchten();
				}
			}
			else if((PORT2->IN & 0x02) == 0x02){
				//Button5
				h5=1;
				if(((PORT1->IN & 0x01) == 0x00)&&((PORT1->IN & 0x02) == 0x00)){
					//Soll_Down=0 und Soll_Up=0
						if(h1||h2||h3||h4){
							PORT1->OMR=0x01; //Soll_Down=1
							leuchten();
						}
						}
						else
							leuchten();
				}
	}

	void IRQ_Hdlr_5(void)
		{
		if((PORT2->IN & 0x400) == 0x400){
			//Button1
			h1=1;
			if(((PORT1->IN & 0x01) == 0x00)&&((PORT1->IN & 0x02) == 0x00)){
				//Soll_Down=0 und Soll_Up=0

				PORT1->OMR=0x02; //Soll_Up=1
				leuchten();
				}
			else leuchten();

		}
		else if((PORT2->IN & 0x40) == 0x40){
			//Button2
			h2=1;
			if(((PORT1->IN & 0x01) == 0x00)&&((PORT1->IN & 0x02) == 0x00)){
				//Soll_Down=0 und Soll_Up=0
				if(h3||h4||h5){
					PORT1->OMR=0x02; //Soll_Up=1
					leuchten();
				}
				else if(h1){
					PORT1->OMR=0x01; //Soll_Down=1
					leuchten();
				}
				else
					leuchten();
			}
		}

		}
	void IRQ_Hdlr_6(void)
	{
		if((PORT2->IN & 0x80) == 0x80){
			//Button4
			h4=1;
			if(((PORT1->IN & 0x01) == 0x00)&&((PORT1->IN & 0x02) == 0x00)){
				//Soll_Down=0 und Soll_Up=0
					if(h5){
						PORT1->OMR=0x02; //Soll_Up=1
						leuchten();
					}
					else if(h1||h2||h3){
						PORT1->OMR=0x01; //Soll_Down=1
						leuchten();
					}
					else
						leuchten();
				}
			}
		else if((PORT2->IN & 0x100) == 0x100){
			//LB4=1
			if(h4&&((PORT1->IN & 0x01) == 0x01)){
				//h4=1 and Soll_Down=1

					h4=0;
					PORT1->OMR=0x10000; //Soll_Down=0
					wait();

					if(h1||h2||h3){
						PORT1->OMR=0x01; //Soll_Down=1
						leuchten();
					}
					else if(h5){
						PORT1->OMR=0x02; //Soll_Up=1
						leuchten();
					}

					else
						leuchten();
					}
			else if(h4&&((PORT1->IN & 0x02) == 0x04)){
					//h4=1 and Soll_Up=1
					h4=0;
					PORT1->OMR=20000; //Soll_Up=0
					wait();

					if(h5){
						PORT1->OMR=0x02; //Soll_Up=1
						leuchten();
					}
					else if(h1||h2||h3){
						PORT1->OMR=0x01; //Soll_Down=1
						leuchten();
						}
					else
						leuchten();
				}
			else
				leuchten();

		}
	}
	void wait(){
		//Wait time between floors
		int i;

		do{
			i=i+1;
		} while(i<9999);
	}
	void leuchten(){
		while(1){
			if(setAB==0){
				//setA
				if(h1){
					PORT1->OMR=0x1C0000;
				}
				if(h2){
					PORT1->OMR=0xC0010;
				}
				if(h3){
					PORT1->OMR=0x140008;
				}
				if(h4){
					PORT1->OMR=0x40018;
				}
				if(h5){
					PORT1->OMR=0x180004;
				}
			}
			else if(setAB==1){
				//setB
				if((PORT0->IN & 0x04) == 0x04){
					//LB1
					PORT1->OMR=0x1C0000;
				}
				if((PORT2->IN & 0x01) == 0x01){
					//LB2
					PORT1->OMR=0xC0010;
				}
				if((PORT0->IN & 0x1) == 0x1){
					//LB3
					PORT1->OMR=0x140008;
				}
				if((PORT2->IN & 0x100) == 0x100){
					//LB4
					PORT1->OMR=0x40018;
				}
				if((PORT2->IN & 0x10)==0x10){
					//LB5
					PORT1->OMR=0x180004;
				}
			}
		}
	}

