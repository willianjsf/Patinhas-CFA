import React, { useEffect, useState } from "react";
import { Image, ScrollView, StyleSheet, Text, View } from "react-native";
import useServerIP from "./useServerIP";
import { Ionicons } from "@expo/vector-icons";
import {calcularMediaPassos, estimarPassosDiarios} from "./Estimador";

const PetStatus = (pet:any) => {
  const [passos, setPassos] = useState(0);

  // Obtém a data atual em BRT (UTC-3)
  const hojeBRT = new Date(new Date().getTime() - 3 * 60 * 60 * 1000).toISOString().split("T")[0];

  // useEffect(() => {
  //   const getPassos = async () => {
  //     try {
  //       const ip = useServerIP();
  //       const response = await fetch("http://" + ip + ":8080/passosDia?dia=" + hojeBRT);
  //       const data = await response.json();
  //       if(data.num_passos === undefined)
  //         setPassos(0);
  //       else
  //         setPassos(data.num_passos);
  //     } catch (error) {
  //       console.error("Erro ao buscar passos:", error);
  //       setPassos(0);
  //     }
  //   };

  //   getPassos();

  // }, []);

  useEffect(() => {
    const getPassos = async () => {
      try {
        // Adicionando um timestamp na URL para evitar que o navegador use cache antigo
        const timestamp = new Date().getTime(); 
        const ip = useServerIP();
        
        // Note que adicionei "&t=" + timestamp no final
        const response = await fetch("http://" + ip + ":8080/passosDia?dia=" + hojeBRT + "&t=" + timestamp);
        
        const data = await response.json();
        
        // Só atualiza o estado se o valor for diferente para economizar processamento
        if(data.num_passos !== undefined) {
             setPassos(prevPassos => {
                 // Se o valor novo for igual ao antigo, retorna o antigo (não renderiza nada)
                 if (prevPassos === data.num_passos) return prevPassos;
                 // Se for diferente, atualiza
                 return data.num_passos;
             });
        }
      } catch (error) {
        console.error("Erro ao buscar passos:", error);
        // Opcional: não zerar os passos em caso de erro de rede momentâneo
        // setPassos(0); 
      }
    };

    // 1. Chama imediatamente ao abrir
    getPassos();

    // 2. Configura um intervalo para chamar a cada 2000 milissegundos (2 segundos)
    const intervalo = setInterval(() => {
        getPassos();
    }, 2000);

    // 3. Limpeza (Importante!): Quando você fecha a tela, para de chamar o servidor
    return () => clearInterval(intervalo);

  }, []); // Mantém o array vazio aqui
  

  const estimativa = estimarPassosDiarios(pet);
  const passosIdeal = estimativa.estimate;

  return (
    <ScrollView contentContainerStyle={styles.container}>
      
      {
        Number(passos) < 30 ? 
          <Text style={[styles.nomeMid, {fontFamily: 'Montserrat_700Bold'}]}>{pet.pet.nome} precisa de atenção!</Text> : 
          <Text style={[styles.nomeHigh, {fontFamily: 'Montserrat_700Bold'}]}>{pet.pet.nome} está indo muito bem!</Text>
      }

      <Text style={[styles.subtitulo, { fontFamily: 'Montserrat_400Regular' }]}>
        “Brinque um pouco comigo”{"\n"}
        <Text style={{ fontWeight: "300" }}>Opa! Quem disse isso?</Text>
      </Text>

      <Image
        source={{ uri: pet.pet.fotoSrc }}
        style={styles.foto}
      />
{/* 
      <View style={[styles.progressBar, {borderWidth: 2, borderColor: '#c0c0c0ff'}]}>
        <View style={[Number(passos) < 40 ? styles.progressFillMid : styles.progressFillHigh, { width: `${(Number(passos) / 30) * 100}%` }]} />
      </View> */}

      <View style={[styles.progressBar, {borderWidth: 2, borderColor: '#c0c0c0ff'}]}>
        <View style={[Number(passos) < 100 ? styles.progressFillMid : styles.progressFillHigh, { width: `${(Number(passos) / 500) * 100}%` }]} />
        <View style={[Number(passos) >= 100 ? styles.progressFillHigh : styles.progressFillHigh, { width: `${(Number(passos) / 500) * 100}%` }]} />
      </View>

      <Text style={[styles.passos, { fontFamily: 'Montserrat_400Regular' }]}>{passos} passos</Text>

      <View style={styles.infoBox}>
        <Text style={styles.title}>Dados do seu pet</Text>

        <View style={styles.editIconContainer}>
          <Ionicons name="pencil" size={20} color="#fff" />
        </View>
        {/* <View style={{width: 200, flexDirection: 'row', alignItems: 'center', justifyContent: 'space-between'}}>
          <Text style={{fontFamily: 'Montserrat_400Regular'}}>Dados do seu pet</Text>
          <Ionicons size={23} name='create-outline' color={'black'} />
        </View> */}
        
        <Text style={[styles.infoText, {fontFamily: 'Montserrat_400Regular', fontWeight: 600}]}>Média de passos do {pet.pet.nome}: {calcularMediaPassos(pet, null).toFixed(1)} </Text>
        <Text style={[styles.infoText, {fontFamily: 'Montserrat_400Regular', fontWeight: 600}]}>Ideal de passos por dia: {passosIdeal} </Text>
        
        <Text style={[styles.infoText, {fontFamily: 'Montserrat_400Regular'}]}>{"\n"}Nome: {pet.pet.nome}</Text>
        <Text style={[styles.infoText, {fontFamily: 'Montserrat_400Regular'}]}>Peso (kg): {pet.pet.pesoKg}</Text>
        <Text style={[styles.infoText, {fontFamily: 'Montserrat_400Regular'}]}>Porte: {pet.pet.porte}</Text>
        <Text style={[styles.infoText, {fontFamily: 'Montserrat_400Regular'}]}>Idade (anos): {pet.pet.idadeAnos}</Text>
        <Text style={[styles.infoText, {fontFamily: 'Montserrat_400Regular'}]}>Energia: {pet.pet.energia}</Text>
        <Text style={[styles.infoText, {fontFamily: 'Montserrat_400Regular'}]}>Castrado: {pet.pet.castrado ? 'sim' : 'não'}</Text>
        <Text style={[styles.infoText, {fontFamily: 'Montserrat_400Regular'}]}>Tem problema de saude: {pet.pet.temProblemaSaude ? 'sim' : 'não'}</Text>
        <Text style={[styles.infoText, {fontFamily: 'Montserrat_400Regular'}]}>Objetivo: {pet.pet.objetivo}</Text>
        <Text style={[styles.infoText, {fontFamily: 'Montserrat_400Regular'}]}>Ambiente ruim: {pet.pet.ambienteRuim}</Text>
        <Text style={[styles.infoText, {fontFamily: 'Montserrat_400Regular'}]}>Cor: {pet.pet.cor}</Text>

        <Text style={[{fontFamily: 'Montserrat_400Regular', color:'white', fontSize:13}]}>{"\n"} * o ideal de passos é calculado com base nas características do seu pet!</Text>
      </View>

    </ScrollView>
  );
};

const styles = StyleSheet.create({
  container: {
    alignItems: "center",
    paddingVertical: 40,
  },

  nomeMid: {
    fontSize: 30,
    fontWeight: '700',
    color: "#d9e021",
    textAlign: "center",
    marginBottom: 6,
    paddingHorizontal: 40,
  },

  nomeHigh: {
    fontSize: 26,
    fontWeight: "700",
    color: "#57c448",
    textAlign: "center",
    marginBottom: 6,
    paddingHorizontal: 40,
  },

  subtitulo: {
    fontSize: 16,
    color: "#999999ff",
    textAlign: "center",
    marginBottom: 20,
  },

  foto: {
    width: 280,
    height: 280,
    borderRadius: 160,
    marginBottom: 20,
  },

  progressBar: {
    width: 260,
    height: 16,
    backgroundColor: "#EFEFEF",
    borderRadius: 8,
    overflow: "hidden",
    marginBottom: 16,
  },

  progressFillMid: {
    height: "100%",
    backgroundColor: "#E3EB3A",
    borderRadius: 6,
  },

  progressFillLow: {
    height: "100%",
    backgroundColor: "red",
    borderRadius: 6,
  },

  progressFillHigh: {
    height: "100%",
    backgroundColor: "#57c448",
    borderRadius: 8,
  },

  passos: {
    fontSize: 36,
    fontWeight: "900",
    color: "#000",
    marginBottom: 8,
  },

  infoBox: {
    backgroundColor: 'rgba(209, 209, 209, 0.82)',  // fundo escuro com transparência
    padding: 16,
    borderRadius: 16,
    marginVertical: 20,
    width: '90%',
    alignSelf: 'center',
    shadowColor: '#000',
    shadowOpacity: 0.25,
    shadowOffset: { width: 0, height: 4 },
    shadowRadius: 8,
    elevation: 6, // sombra no Android
  },

  infoText: {
    color: '#242424ff',
    fontSize: 16,
    marginVertical: 2,
    lineHeight: 22,
  },

  title: {
    fontSize: 22,
    fontWeight: '600',
    color: '#585858ff',
    marginBottom: 10,
    textAlign: 'center',
  },

  editIconContainer: {
    position: 'absolute',
    top: 12,
    right: 12,
  }
});

export default PetStatus;
