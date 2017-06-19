//          Copyright Jeremy Ruizenaar 2016 - 2017
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


///@file 
//#include "hwlib.hpp"
///library voor interfacing met rtc chip
//
///deze klasse maakt een rtc object uit een chip op een adress en een i2c bus


// ADDED BY VINCE
typedef uint8_t byte;
typedef uint8_t fast_byte;
typedef int16_t word;
typedef int32_t double_word;
// ADDED BY VINCE

class i2cRTClib {
private:
	
	int uren;
	int minuten; 
	int seconden;
	int week;
	int dag;
	int maand;
	int jaar;
	
	

	fast_byte  a = 0x68; //adres en naam van de chip ds1307 01101000
	byte data[8] = {0,0,0,0,0,0,0}; 
	byte  data_get[7];
	
	
	
	hwlib::i2c_bus_bit_banged_scl_sda & i2c;  // i2c bus
	// benodigd heden 2c buss 
	
	
		byte dec_Bcd(byte waarde)														//deze functie zet een ingevoerde dec waarde om in de bcd waarde
		{
			return( (waarde/10*16) + (waarde%10) );
		}
		
		byte bcd_Dec(byte waarde)														//deze functie zet de ingelezen bcd waarde uit de chip om byte
		{
			return( (waarde/16*10) + (waarde%16) );
		}
	


public:

	///default constructor
	//
	///creert rtc object zonder geinitialiseerde waardes
	i2cRTClib(hwlib::i2c_bus_bit_banged_scl_sda & i2c):
			i2c( i2c)
			{}
	/// constructor 
	//
	/// creert een rtc object dmv een i2c-bus
	/// met als argumenten de waarde van de tijd die de timer moet aannemen
	i2cRTClib(hwlib::i2c_bus_bit_banged_scl_sda & i2c, int sec =0, int min=0, int uur=0, int dag_week=0, int dag_maand=0, int maand=0, int jaar=0):
			i2c( i2c)
		{
			set_seconden(sec);
			set_minuten(min);
			set_uren(uur);
			set_dag_week(dag_week);
			set_dag_maand(dag_maand);
			set_maand(maand);
			set_jaar(jaar);
			
			}
		

	/// zet de tijd
	//
	/// deze functie schrijft deze waardes in het rtc object 
	virtual void set_time(int sec, int min, int uur, int dag_week, int dag_maand, int maand, int jaar){ // 
		//voer uren en minuten in op display  haaal deze waardes op en stuur ze naar rtc
		//i2c.write(a , data_first, 1);
		
		data[0] = 0;
		data[1]= dec_Bcd(sec);
		data[2] = dec_Bcd(min) ;
		data[3] = dec_Bcd(uur);;
		data[4] = dec_Bcd(dag_week) ;
		data[5] = dec_Bcd(dag_maand) ;
		data[6] = dec_Bcd(maand) ;
		data[7] = dec_Bcd(jaar) ;
		i2c.write(a , data, 8);
	}
	///get tijd (array)
	//
	///deze functie returned een array met daarin de waardes van de rtc
	///[0] = sec [1] =min [2] = uur [3] = dag van week [4]  = dag van maand [5] = maand [6] = jaar
	///
	///
	
	byte get_tijd(){ // haal uren op uit rtc
		i2c.write(a,  data , 1);
		i2c.read(a, data_get, 7);
		data_get[0]  = bcd_Dec(data_get[0]);
		data_get[1] = bcd_Dec(data_get[1] & 0x7f);
		data_get[2] = bcd_Dec(data_get[2] & 0x3f);
		data_get[3] = bcd_Dec(data_get[3]);
		data_get[4] = bcd_Dec(data_get[4]);
		data_get[5] = bcd_Dec(data_get[5]);
		data_get[6] = bcd_Dec(data_get[6]);
		
		
		hwlib::wait_ms( 1 );
		return data_get[7];
		/*hwlib::cout << "bcd " << seconden << "  seconden    "  << "byte   " << int(data_get[0]) << " NC " <<data[1] << "\n";
		//hwlib::wait_ms( 1 );
		hwlib::cout << "bcd " << minuten  << " minuten     "  << "byte   "  << int(data_get[1]) << " NC" <<data[2]  << "\n";
		//hwlib::wait_ms( 1 );
		hwlib::cout << "bcd " << uren  << "  uren    "  << "byte   "   << int(data_get[2]) << " NC  " <<data[3]  <<  "\n";
		//hwlib::wait_ms( 1 );
		hwlib::cout << "bcd " << week  << " week "   <<   "byte   " <<  int(data_get[3]) << " NC " <<data[4]  <<  "\n";
		hwlib::wait_ms( 1 );
		hwlib::cout << "bcd " << dag  << "   dag "    << "byte   "  <<  int(data_get[4]) << " NC " <<data[5]  <<  "\n";
		hwlib::wait_ms( 1 );
		hwlib::cout << "bcd " << maand  << "  maand  "   << "byte   "   <<  int(data_get[5]) << " NC " <<data[6]  <<  "\n";
		hwlib::wait_ms( 1 );
		hwlib::cout << "bcd " << jaar  << " jaar     "   << "byte   "   <<  int(data_get[6]) << "  NC " <<data[7]  <<  "\n";*/
		
		
		
		
		
	}
	
	///get seconden
	//
	///deze retourneert het aantal seconden
	virtual  int get_seconden(){  // haal minuten op uit rtc
	
		byte rtcRegister[1] = {0};  													//schrijf waarde op adrespointer op dat geheugen te zetten
		byte buffer[1] = {0};
		i2c.write(a,  rtcRegister , 1); 												//schrijf waarde in weg naar chip
		i2c.read(a, buffer, 1);															// lees byte op dat adres in
		return bcd_Dec(buffer[0]);
	}
	///set seconden
	//
	///deze functie zet het aantal seconden
	virtual  void set_seconden(int x){  // haal minuten op uit rtc
		byte rtcRegister[2] = {0,dec_Bcd(x)}; 
		i2c.write(a,  rtcRegister , 2);
	}
	///get minuten
	//
	///deze functie retourneert het aantal minuten
	int get_minuten(){  // haal minuten op uit rtc
		byte rtcRegister[1] = {1}; 
		byte buffer[1] = {0};
		i2c.write(a,  rtcRegister , 1);
		i2c.read(a, buffer, 1);
		return bcd_Dec(buffer[0]);
	}
	///set minuten
	//
	///deze functie set de huidige minuten
	virtual  void set_minuten(int x){  // haal minuten op uit rtc
		byte rtcRegister[2] = {1,dec_Bcd(x)}; 
		i2c.write(a,  rtcRegister , 2);
	}
	///get uren
	//
	///deze functie retourneert het aantal uren
	virtual int get_uren(){  // haal seconden op uit rtc
		byte rtcRegister[1] = {2}; 
		byte buffer[1] = {0};
		i2c.write(a,  rtcRegister , 1);
		i2c.read(a, buffer, 1);
		return bcd_Dec(buffer[0]);
	}
	///set uren
	//
	///deze functie set het huidige aantal uren
	virtual  void set_uren(int x){  // haal seconden op uit rtc
		byte rtcRegister[2] = {2,dec_Bcd(x)}; 
		i2c.write(a,  rtcRegister , 2);
	}
	///toggle 24h/12h mode
	//
	///deze functie toggled het 24h/12h bit
	virtual  void toggle_24h_set(int toggle){
		if(toggle == 1){
			byte b = dec_Bcd(get_uren()) | 0x40;

			byte rtcRegister[2] ={2,b};
			i2c.write(a,  rtcRegister , 2);
		}
		else if (toggle ==0){
			byte b = dec_Bcd(get_uren()) & 0x2f;
			byte rtcRegister[2] ={2,b};
			i2c.write(a,  rtcRegister , 2);
		}
	}
	///get dag week
	//
	///deze functie retourneert de huidige dag van de week
	virtual  int get_dag_week(){  // haal seconden op uit rtc
		byte rtcRegister[1] = {3}; 
		byte buffer[1] = {0};
		i2c.write(a,  rtcRegister , 1);
		i2c.read(a, buffer, 1);
		return bcd_Dec(buffer[0]);
	}
	///set dag van de week
	//
	///de functie zet de huidge dag van de week
	virtual  void set_dag_week(int x){  // haal seconden op uit rtc
		byte rtcRegister[2] = {3,dec_Bcd(x)}; 
		i2c.write(a,  rtcRegister , 2);
	}
	///get dag van de maand
	//
	///deze functie retourneert de  dag van de maand
	virtual int get_dag_maand(){  // haal seconden op uit rtc
		byte rtcRegister[1] = {4}; 
		byte buffer[1] = {0};; 
		i2c.write(a,  rtcRegister , 1);
		i2c.read(a, buffer, 1);
		return bcd_Dec(buffer[0]);
	}
	///zet dag maand
	//
	///deze functie set de huidige dag van de maand
	virtual void set_dag_maand(int x){  // haal seconden op uit rtc
		byte rtcRegister[2] = {4,dec_Bcd(x)}; 
		i2c.write(a,  rtcRegister , 2);
	}
	///get maand
	//
	///deze functie retourneert de huidige maand
	virtual int get_maand(){  // haal seconden op uit rtc
		byte rtcRegister[1] = {5}; 
		byte buffer[1] = {0};
		i2c.write(a,  rtcRegister , 1);
		i2c.read(a, buffer, 1);
		return bcd_Dec(buffer[0]);
	}
	///set maand
	//
	///deze functie set de huidige maand
	virtual void set_maand(int x){  // haal seconden op uit rtc
		byte rtcRegister[2] = {5,dec_Bcd(x)}; 
		i2c.write(a,  rtcRegister , 2);
	}
	///get jaar
	//
	///deze functie retourneert het huidige jaren
	virtual int get_jaar(){  // haal seconden op uit rtc
		byte rtcRegister[1] = {6}; 
		byte buffer[1] = {0}; 
		i2c.write(a,  rtcRegister , 1);
		i2c.read(a, buffer, 1);
		return bcd_Dec(buffer[0]);
	}
	///set jaar
	//
	///deze functie set het huidige jaar
	virtual void set_jaar(int x){  // haal seconden op uit rtc
		byte rtcRegister[2] = {6,dec_Bcd(x)}; 
		i2c.write(a,  rtcRegister , 2);
	}
	///set adress
	//
	///deze functie schrijft waarde op adress x
	virtual void set_adres_x(byte x, byte waarde){
		byte rtcRegister[2] = {x,waarde}; 
		i2c.write(a,  rtcRegister , 2);
	}
	///get adress
	//
	///deze functie haalt een byte op van geheugenplaats x
	virtual int get_adres_x(byte x){
		byte rtcRegister[1] = {x}; 
		byte buffer[1] = {0}; 
		i2c.write(a,  rtcRegister , 1);
		i2c.read(a, buffer, 1);
		return buffer[0];
	}
	///oscilator set
	//
	///deze funcit set de oscilator on bit in het controleregister door aanroep met een 1 of 0 als parametet
	virtual void osc_set(int toggle){
		if(toggle ==1){
			byte b = dec_Bcd(get_adres_x(7)) | 0x80;
			
			byte rtcRegister[2] ={7,b};
			i2c.write(a,  rtcRegister , 2);
		}
		else{
			byte b = dec_Bcd(get_adres_x(7)) & 0x7f;
			byte rtcRegister[2] ={7,b};
			i2c.write(a,  rtcRegister , 2);
		}
	}
	///swqwe set
	//
	///deze functie set het sqwe bit in het controleregister dooraanroep met een 0 of 1
	virtual void sqwe_set(int toggle){
		if(toggle ==1){
			byte b = dec_Bcd(get_adres_x(7)) | 0x10;
			byte rtcRegister[2] ={7,b};
			i2c.write(a,  rtcRegister , 2);
		}
		else{
			byte b = dec_Bcd(get_adres_x(7)) & 0xef;
			byte rtcRegister[2] ={7,b};
			i2c.write(a,  rtcRegister , 2);
		}
	}
	///rs0 set
	//
	///deze functie toggled het rs0 bit in het controle register
	virtual void rs0_set (int toggle){
		if(toggle ==1){
			byte b = dec_Bcd(get_adres_x(7)) | 0x01;
			byte rtcRegister[2] ={7,b};
			i2c.write(a,  rtcRegister , 2);
		}
		else{
			byte b = dec_Bcd(get_adres_x(7)) & 0xfe;
			byte rtcRegister[2] ={7,b};
			i2c.write(a,  rtcRegister , 2);
		}
	}
	///rs0 set
	//
	///deze functie toggled het rs1 bit in het controle register
	virtual void rs1_set(int toggle){
		if(toggle ==1){
			byte b = dec_Bcd(get_adres_x(7)) | 0x20;
			byte rtcRegister[2] ={7,b};
			i2c.write(a,  rtcRegister , 2);
		}
		else{
			byte b = dec_Bcd(get_adres_x(7)) & 0xfd;
			byte rtcRegister[2] ={7,b};
			i2c.write(a,  rtcRegister , 2);
		}
	}
	
	
};