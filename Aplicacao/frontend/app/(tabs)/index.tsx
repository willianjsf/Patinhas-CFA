import { useFonts, Montserrat_400Regular, Montserrat_700Bold } from '@expo-google-fonts/montserrat';
import CarrosselPets from "@/patinhas-components/CarrosselPets";
import Header from "@/patinhas-components/Header";
import { View } from "react-native";

const index = () => {
    const [fontsLoaded] = useFonts({
        Montserrat_400Regular,
        Montserrat_700Bold,
    });

    return (
        <>
            <Header></Header>
            <CarrosselPets></CarrosselPets> 
        </>
    );
}

export default index;