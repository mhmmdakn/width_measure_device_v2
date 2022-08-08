#pragma once

#include <EEPROM.h>

///////Register Start Address///////
#define WIFI_BEGIN 1
#define WIFI_CONFIG 100
#define ETH_CONFIG 200
#define DEVICE_CONFIG 300
/////////////////////////////////////

///////Register END Address///////
#define WIFI_BEGIN_END 99
#define WIFI_CONFIG_END 199
#define ETH_CONFIG_END 299
#define DEVICE_CONFIG_END 399
////////////////////////////////////

#define wifi_begin_lenght 16
#define wifi_config_lenght 4
#define eth_config_lenght 4


 enum{
  sta_ssid=0,
  sta_password=32,
  ap_ssid=64,
  ap_password,
  log_username,
  log_password,  
  wifi_begin_size 
  };///WIFI_BEGIN
  
  enum{
  sta_ip,
  sta_subnet,
  sta_gateway,
  sta_dns,
  wifi_ntp,    
  wifi_config_size    
  };///WIFI_CONFIG
   
  enum{
  eth_ip,
  eth_subnet,  
  eth_gateway,
  eth_dns,
  eth_ntp,
  eth_config_size    
  };///ETHERNET_CONFIG
  
enum{
  referans_uzunluk, 
  factory_settings,
  device_config_size
  };///DEVICE_CONFIG
  
	enum{	
    step1_pos_meas_mm=0,//ushort
    step1_pos_actual_mm=2,//ushort
    step2_pos_meas_mm=4,//ushort
    step2_pos_actual_mm=6,//ushort
    step_position_mm=8, //ushort
    en_mm=10,
	end_of_work,
	mwidth_val,
	cwidth_val,
	number_of_samples,
	fabric_ref,
  measured,	
  fault,  
  run_stop,
	wifi_ok,
	ap_sta_mod,
	updt,	
	width_vals_start,
	device_register_size=1000
	};///DEVICE_REGISTER





class  RegisterCollection : public  EEPROMClass{

    public :
    bool begin(size_t size);
    

};

extern RegisterCollection RA;
extern EEPROMClass RK;

extern word device_register[device_register_size];
extern word device_config[device_config_size];
extern String wifi_begin[wifi_begin_size];
extern byte wifi_config[wifi_config_size][wifi_config_lenght];
extern byte eth_config[eth_config_size][eth_config_lenght];

extern map<int,double*> ht_param_anlik;
extern map<int,JSONVar*> ht_param_kalici;

 void register_init();
  

 

// class Object
// {
// protected:
//     Object(){};
//     virtual ~Object(){};

// public:
//     virtual std::string toString() const {return("");};
// }; // eo class Object

// class Int : public Object
// {
// private:
//     int m_nVal;

// public:
//     Int(int _val = 0) : m_nVal(_val){};
//     Int(const Int& _rhs) : m_nVal(_rhs.m_nVal){};
//     virtual ~Int(){};

//     // operators
//     operator int() const {return(m_nVal);}
//     bool operator == (const Int& _rhs) const {return(m_nVal == _rhs.m_nVal);};
//     bool operator == (int _val) const {return(m_nVal == _val);};
//     Int& operator = (const Int& _rhs) {m_nVal = _rhs.m_nVal; return(*this);};
//     Int& operator = (int _val) {m_nVal = _val; return(*this);};
//     // .... and all the other operators

//     // overrides
//     virtual std::string toString() const
//     {
//     //    std::ostringstream oss;
//     //    oss << m_nVal;
//     //    return(oss.str());
//     };
// }; // eo class Int


// template<class T> class IntegralType : public Object
// {
// private:
//     T m_Val;

// public:
//     // rest of class looks the same
// }; // eo class IntegralType<>


// typedef int Int;
// typedef IntegralType<short> Short;
// typedef IntegralType<long> Long;

// template<class T> class IEquitable
// {
// public:
//     virtual Equals(T _other) = 0;
// };   // eo class IEquitable<>

