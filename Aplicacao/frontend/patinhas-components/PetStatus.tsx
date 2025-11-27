import React, { useEffect, useState } from "react";
import { Image, ScrollView, StyleSheet, Text, View } from "react-native";
import useServerIP from "./useServerIP";
import { Ionicons } from "@expo/vector-icons";
import {calcularMediaPassos, estimarPassosDiarios} from "./Estimador";

const PetStatus = (pet:any) => {
  const [passos, setPassos] = useState(0);

  // Obtém a data atual em BRT (UTC-3)
  const hojeBRT = new Date(new Date().getTime() - 3 * 60 * 60 * 1000).toISOString().split("T")[0];

  useEffect(() => {
    const getPassos = async () => {
      try {
        const ip = useServerIP();
        const response = await fetch("http://" + ip + ":8080/passosDia?dia=" + hojeBRT);
        const data = await response.json();
        if(data.num_passos === undefined)
          setPassos(0);
        else
          setPassos(data.num_passos);
      } catch (error) {
        console.error("Erro ao buscar passos:", error);
        setPassos(0);
      }
    };

    getPassos();

  }, []);

  const estimativa = estimarPassosDiarios(pet);
  const passosIdeal = estimativa.estimate;

  return (
    <ScrollView contentContainerStyle={styles.container}>
      
      {
        Number(passos) < 30 ? 
          <Text style={[styles.nomeMid, { fontFamily: 'Montserrat_400Regular'}]}>{pet.pet.nome} precisa de atenção</Text> : 
          <Text style={[styles.nomeHigh, {fontFamily: 'Montserrat_400Regular'}]}>{pet.pet.nome} está indo muito bem</Text>
      }

      <Text style={[styles.subtitulo, { fontFamily: 'Montserrat_400Regular' }]}>
        “Brinque um pouco comigo”{"\n"}
        <Text style={{ fontWeight: "300" }}>Opa! Quem disse isso?</Text>
      </Text>

      <Image
        source={{ uri: pet.pet.fotoSrc }}
        style={styles.foto}
      />

      <View style={[styles.progressBar, {borderWidth: 1, borderColor: '#000'}]}>
        <View style={[Number(passos) < 40 ? styles.progressFillMid : styles.progressFillHigh, { width: `${(Number(passos) / 30) * 100}%` }]} />
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
    paddingVertical: 20,
  },

  nomeMid: {
    fontSize: 26,
    fontWeight: "700",
    color: "#FFB800",
    textAlign: "center",
    marginBottom: 6,
  },

  nomeHigh: {
    fontSize: 26,
    fontWeight: "700",
    color: "green",
    textAlign: "center",
    marginBottom: 6,
  },

  subtitulo: {
    fontSize: 16,
    color: "#555",
    textAlign: "center",
    marginBottom: 20,
  },

  foto: {
    width: 190,
    height: 190,
    borderRadius: 90,
    marginBottom: 18,
  },

  progressBar: {
    width: 200,
    height: 12,
    backgroundColor: "#EFEFEF",
    borderRadius: 6,
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
    backgroundColor: "green",
    borderRadius: 6,
  },

  passos: {
    fontSize: 36,
    fontWeight: "700",
    color: "#000",
    marginBottom: 8,
  },

  infoBox: {
    backgroundColor: 'rgba(0, 0, 0, 0.82)',  // fundo escuro com transparência
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
    color: '#FFF',
    fontSize: 16,
    marginVertical: 2,
    lineHeight: 22,
  },

  title: {
    fontSize: 22,
    fontWeight: '600',
    color: '#FFF',
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
