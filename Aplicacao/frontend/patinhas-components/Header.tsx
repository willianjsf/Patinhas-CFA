import { View, Image, Text } from "react-native";
import { Ionicons } from '@expo/vector-icons';
import { Montserrat_400Regular } from "@expo-google-fonts/montserrat";

const Header = () => {
    return(
        <View style={{width:'100%', flexDirection:'row', alignItems:'center', justifyContent:"space-between", backgroundColor:'white'}}>
            <Image source={require("../assets/logo.png")} style={{width:150, height:100, marginLeft:10}}></Image>
            <View style={{flexDirection: 'row', alignItems:'center', marginRight:30}}>
                <Text style={{color: "grey", marginRight:6, fontFamily: 'Montserrat_400Regular'}}>90%</Text>
                <Ionicons name="battery-full" size={40} color="grey" />
            </View>
        </View>
    );
}

export default Header;