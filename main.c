#include <stdio.h>
#include <math.h>

#define MIN 0
#define NOMINAL 1
#define MAX 2

float cap_comercial(float C);

char capacitores[14]={10, 12, 15, 18, 22, 25, 27, 33, 39, 47, 56, 68, 82, 100};

void main(){
    int f;
    float VRede[3], VarRede;
    float Ripple[3], VRet;
    float Io, C, Cap;
    char escolha='e';

    float Po[3], VcMin[3], VcMax[3], Vo[3], Vefs[3], Iefs[3], S[3], RT;

    printf("DIMENSIONAMENTO DO TRAFO");
    //Informações sobre a tensão da rede
    printf("\n->Tensão eficaz nominal da rede(V): ");
    scanf("%f", &VRede[NOMINAL]);
    printf("->Variação máxima da tensão da rede(%%): ");
    scanf("%f", &VarRede);

    //Fequência da rede
    printf("->Frequência da rede(Hz): ");
    scanf("%d", &f);

    //Informações de tensão depois da retificação
    printf("->Tensão média mínima(V): ");
    scanf("%f", &Vo[MIN]);
    printf("->Ripple máximo de tensão(%%): ");
    scanf("%f", &Ripple[MIN]);

    //Queda de tensão na retificação
    printf("->Queda de tensão na retificação(V): ");
    scanf("%f", &VRet);

    //Corrente máxima depois da retificação
    printf("->Corrente contínua máxima(A): ");
    scanf("%f", &Io);

    //-------------------------------------------------------------------------------------------------//
    
    //Tensão mínima da rede
    VRede[MIN]=((100-VarRede)/100)*VRede[NOMINAL];
    //Tensão máxima da rede
    VRede[MAX]=((100+VarRede)/100)*VRede[NOMINAL];

    //Tensão mínima no Capacitor no pior cenário (Tensão Média Mínima)
    VcMin[MIN]=Vo[MIN]*(200-2*Ripple[MIN])/(200-Ripple[MIN]);
    //Tensão máxima no Capacitor no pior cenário (Tensão Média Mínima)
    VcMax[MIN]=Vo[MIN]*200/(200-Ripple[MIN]);
    //Potência ativa para menor tensão média
    Po[MIN]=Vo[MIN]*Io;

    //Calculo da capacitância mínima
    C=Po[MIN]/(f*(pow(VcMax[MIN],2)-pow(VcMin[MIN],2)));
    Cap=cap_comercial(C);

    printf("\n->Capacitância miníma calculada: %.1e F", C);
    printf("\n->Capacitância comercial sugerida %.1e F", Cap);

    while (!(escolha=='s' || escolha=='S' || escolha=='n' || escolha=='N'))
    {
        printf("\n\n->Deseja usar a capacitância sugerida? [s/n] \n->");
        scanf("%s", &escolha);
    }
    
    if(escolha=='n' || escolha=='N'){
        printf("\n->Digite a capacitância escolhida: ");
        scanf("%f", &Cap);
    }

    escolha='e';

    //-------------------------------------------------------------------------------------------------//

    VcMax[MIN]=Vo[MIN]+Io/(4*f*Cap);
    Vefs[MIN]=(VcMax[MIN]+VRet)/sqrt(2);
    RT=VRede[MIN]/Vefs[MIN];

    Vefs[NOMINAL]=VRede[NOMINAL]/RT;

    printf("\n->Tensão do secundário calculada: %.2f V", Vefs[NOMINAL]);
    printf("\n->Digite a tensão escolhida: ");
    scanf("%f", &Vefs[NOMINAL]);

    escolha='e';

    //-------------------------------------------------------------------------------------------------//

    RT=VRede[NOMINAL]/Vefs[NOMINAL];
    VcMax[NOMINAL]=Vefs[NOMINAL]*sqrt(2)-VRet;
    VcMin[NOMINAL]=VcMax[NOMINAL]-Io/(2*f*Cap);
    Vo[NOMINAL]=(VcMax[NOMINAL]+VcMin[NOMINAL])/2;
    Po[NOMINAL]=Vo[NOMINAL]*Io;
    Iefs[NOMINAL]=2*Io*sqrt(M_PI/(3*acos(VcMin[NOMINAL]/VcMax[NOMINAL])));
    S[NOMINAL]=Vefs[NOMINAL]*Iefs[NOMINAL];
        
    Vefs[MIN]=VRede[MIN]/RT;
    VcMax[MIN]=Vefs[MIN]*sqrt(2)-VRet;
    VcMin[MIN]=VcMax[MIN]-Io/(2*f*Cap);
    Vo[MIN]=(VcMax[MIN]+VcMin[MIN])/2;
    Po[MIN]=Vo[MIN]*Io;
    Iefs[MIN]=2*Io*sqrt(M_PI/(3*acos(VcMin[MIN]/VcMax[MIN])));
    S[MIN]=Vefs[MIN]*Iefs[MIN];

    Vefs[MAX]=VRede[MAX]/RT;
    VcMax[MAX]=Vefs[MAX]*sqrt(2)-VRet;
    VcMin[MAX]=VcMax[MAX]-Io/(2*f*Cap);
    Vo[MAX]=(VcMax[MAX]+VcMin[MAX])/2;
    Po[MAX]=Vo[MAX]*Io;
    Iefs[MAX]=2*Io*sqrt(M_PI/(3*acos(VcMin[MAX]/VcMax[MAX])));
    S[MAX]=Vefs[MAX]*Iefs[MAX];

    ///-------------------------------------------------------------------------------------------------//

    printf("\n\tV REDE\tVo\tIo\tPo\tVefs\tIefs\tS\tC");
    printf("\nMIN\t%.2fV\t%.2fV\t%.3fA\t%.2fW\t%.2fV\t%.3fA\t%.2fVA\t%fF", VRede[MIN], Vo[MIN], Io, Po[MIN], Vefs[MIN], Iefs[MIN], S[MIN], Cap);
    printf("\nNOM\t%.2fV\t%.2fV\t%.3fA\t%.2fW\t%.2fV\t%.3fA\t%.2fVA\t%fF", VRede[NOMINAL], Vo[NOMINAL], Io, Po[NOMINAL], Vefs[NOMINAL], Iefs[NOMINAL], S[NOMINAL], Cap);
    printf("\nMAX\t%.2fV\t%.2fV\t%.3fA\t%.2fW\t%.2fV\t%.3fA\t%.2fVA\t%fF", VRede[MAX], Vo[MAX], Io, Po[MAX], Vefs[MAX], Iefs[MAX], S[MAX], Cap);
}

float cap_comercial(float capacitancia){
    int dec=0, i=0;
    float comercial;

    comercial=capacitancia;

    while(comercial<10){
        dec++;
        comercial*=10;
    }

    //Encontra o multiplicador mais próximo
    while (!(comercial>=capacitores[i] && comercial<=capacitores[i+1])){
        i++;
    }
    if  ((comercial-capacitores[i]) > (capacitores[i+1]-comercial)){
        i++;
    }

    //Devolve as casas decimais e retorna o valor do capacitor comercial
    comercial=capacitores[i]*pow(10, -dec);
    return comercial;
}