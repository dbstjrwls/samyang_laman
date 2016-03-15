

// Common ExecStep define

#define	COMMON_NOFUNC		5000
#define	COMMON_WAIT		    5010
#define	COMMON_END		    5020
#define	COMMON_RETURN		5030
#define	COMMON_SAVE		    5040
#define	COMMON_CANT		    5050
#define	COMMON_EXEC_ERR		5060
#define	COMMON_REG_WRITE  	5070  

// Main Menu ExecStep define

#define	MODE_A				100
#define	MODE_B  			110
#define	MODE_C				120
#define	MODE_D  			130
#define	MODE_E  			140

unsigned int ExecMode = 0;
unsigned int menu_step;
unsigned int menu_mode;

void command_function(void)
{
	switch (ExecMode)
	{
		case MAIN_MENU 	: main_menu_function();	return;
		case MODE_SET_A : menu_function_A();	return;
		case MODE_SET_B : menu_function_B();	return;
		case MODE_SET_C : menu_function_C();	return;
		case MODE_SET_D : menu_function_D();	return;
		default :
			common_func(ExecMode, COMMON_EXEC_ERR, menu_step);
			return;
	};
}

void menu_function_A(void)
{
	write_txbuf_string("\r\nMenu Function A\r\n");
	tmp_rev = rxd;
	if (tmp_rev == ESC) 
		common_func(MAIN_MENU, COMMON_RETURN, 1);
	else
		write_txbuf_string("\r\nMenu Function A..\r\n");
}

void menu_function_B(void)
{
	write_txbuf_string("\r\nMenu Function B\r\n");
	tmp_rev = rxd;
	if (tmp_rev == ESC)
		common_func(MAIN_MENU, COMMON_RETURN, 1);
	else
		write_txbuf_string("\r\nMenu Function B..\r\n");
}

void menu_function_C(void)
{
	write_txbuf_string("\r\nMenu Function C\r\n");
	tmp_rev = rxd;
	if (tmp_rev == ESC)
		common_func(MAIN_MENU, COMMON_RETURN, 1);
	else
		write_txbuf_string("\r\nMenu Function C..\r\n");
}

void menu_function_D(void)
{
	write_txbuf_string("\r\nMenu Function D\r\n");
	tmp_rev = rxd;
	if (tmp_rev == ESC)
		common_func(MAIN_MENU, COMMON_RETURN, 1);
	else
		write_txbuf_string("\r\nMenu Function D..\r\n");
}

void main_menu_title_display(void)
{
	write_txbuf_string("\r\n Controller Main Memu");
    status_display();
    write_txbuf_string("\r\n---------------------");
    write_txbuf_string("\r\n [ CMD ] Action");
    write_txbuf_string("\r\n---------------------");
    write_txbuf_string("\r\n [ 1 ]");
    write_txbuf_string("\r\n [ 2 ]");
    write_txbuf_string("\r\n [ 3 ]");
	write_txbuf_string("\n\rCS>Select:");   
} 
   
void main_menu_function(void)
{

 switch (menu_step)
 {                                          
 
  case 0:
    menu_step++;
    return;
    
  case 1:
	// monitor_clear();
    main_menu_title_display();
    menu_step++;
    return;
    
  case 2:
    if (rxd == '1')       menu_step = MODE_A;
    else if (rxd == '2')  menu_step = MODE_B;
    else if (rxd == '3')  menu_step = MODE_C;

  case MODE_A:
    menu_change(MODE_SET_A);
    return;
   
  case MODE_B:
    menu_change(MODE_SET_B);
    return;

  case MODE_C:
    menu_change(MODE_SET_C);
    return;

  default:
	write_txbuf_string("\r\n Step : ");
	Display_Realnum(menu_step, 0);
    return;
  }
}

// COMMON MENU Functions                      
char ReturnMode;
short sReturnStep;

void common_func(char mode, short func, short retstep)
{
  ReturnMode  = mode;
  sExecStep   = func;
  sReturnStep = retstep;
  ExecMode    = COMMON_MENU;
}
  
void COMMON_key_function(void)
{
 char result; 

 switch (sExecStep)
 {   
//*************************************************
// Save Message and Return
//*************************************************
  case COMMON_SAVE:
	write_txbuf_string("\r\n Saved");
    DelayStep = 0;
    menu_step++;
    return;
    
  case COMMON_SAVE+1:
    if (step_delay(SEC_1)) menu_step = COMMON_END;
    return;

//*************************************************
// Can't Use
//*************************************************
  case COMMON_CANT:
    line_feeding(2);
	write_txbuf_string("\r\n Now Can't Use");
    menu_step = 1;
    return;    
   
//*************************************************
// Rerutn Main Function
//*************************************************
  case COMMON_RETURN:
    menu_step = sReturnStep;
    ExecMode  = ReturnMode;
    return;

//*************************************************
// End
//*************************************************  
  case COMMON_END:
    DelayStep = 0;
    line_feeding(3);
    menu_step = COMMON_RETURN;
    return;
    
//*************************************************
// End after 2Sec Dealy
//*************************************************  
  case COMMON_WAIT:
    DelayStep = 0;
    menu_step++;
    return;
    
  case COMMON_WAIT+1:
    if (ANY_KEY) menu_step = COMMON_END;
    else if (step_delay(SEC_1*2)) menu_step = COMMON_END;
    return;
 
//*************************************************
// Wait
//*************************************************  
  case COMMON_NOFUNC:
	write_txbuf_string("\r\n Ready");
    menu_step = COMMON_WAIT;
    return;
    
//*************************************************
// Function Run Error
//*************************************************  
  case COMMON_EXEC_ERR:
    sReturnStep = 1;
    DelayStep   = 0;
    menu_step   = COMMON_WAIT;
    return;

  default:
    ReturnMenu  = MAIN_MENU;
    sReturnStep = 1;
    DelayStep   = 0;
    menu_step   = COMMON_WAIT;
    return;
 }
}

// Delay

char step_delay(int time)
{
  char end;
  end = 0;
  if (DelayStep == 0) 
  {
    DelayStep = 1;
    iStepCount = time;
  }
  else if (--iStepCount < 1)
  {
    DelayStep = 0;
    sExecStep++;
    end = 1;
  }
  return end;  
}

void menu_change(unsigned char mode)
{
   menu_step = 0;
   ExecMode  = mode;
}

