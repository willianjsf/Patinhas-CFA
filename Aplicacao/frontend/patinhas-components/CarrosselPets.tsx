import React, { useEffect, useState } from "react";
import { ScrollView, Text, View } from "react-native";
import PetStatus from "./PetStatus";
import useServerIP from "./useServerIP";

const CarrosselPets = () => {
  const [pets, setPets] = useState([]);   // lista de pets
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    const fetchPets = async () => {
      try {
        const ip = useServerIP()
        const response = await fetch("http://" + ip +":8080/animalInfo");
        const data = await response.json();
        setPets(data["dados"]);        // salva a lista de pets
      } catch (error) {
        console.error("Erro ao buscar pets:", error);
      } finally {
        setLoading(false);
      }
    };

    fetchPets();
  }, []); // roda só 1 vez

  // Enquanto carrega
  if (loading) {
    return <Text style={{fontFamily: 'Montserrat_400Regular'}}>Carregando...</Text>;
  }

  // Quando carregar
  return (
    <ScrollView>
      {pets.map((p, index) => (
        <PetStatus key={index} pet={p} />
      ))}
    </ScrollView>
  );
};

export default CarrosselPets;
