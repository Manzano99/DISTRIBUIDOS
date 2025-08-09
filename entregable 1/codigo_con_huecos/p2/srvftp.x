const LENMSG     = 1024;
const LENNOMFILE = 128;

typedef string cadenaL<LENMSG>;
typedef string cadenaS<LENNOMFILE>;

struct param1 {
   cadenaS filename;
   int linea;
};

struct param2 {
   cadenaS filename;
   int lineaini;
   int lineafin;
};

struct listalin {
   cadenaL linea;
   listalin * sgte;
};


union Resultado switch (int caso){
   case 0: bool b;
   case 1: int val;
   case 2: cadenaL linea;
   case 3: listalin * lista;
   case 4: cadenaL err;
};

program SRVFTP{
   version PRIMERA{
     Resultado isfile(cadenaS)=1;
     Resultado isreadable(cadenaS)=2;
     Resultado getnumlines(cadenaS)=3;
     Resultado getline(param1)=4;
     Resultado getlines(param2)=5;
   }=1;
}=0x2023f001;

