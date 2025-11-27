import Header from '@/patinhas-components/Header';
import {Button, Text, TextInput, View, Image} from 'react-native'
import { BleManager } from 'react-native-ble-plx';

// const BTmanager = new BleManager();

export default function newDevice() {
    return(
        <View>
            <Header></Header>

            <View style={{backgroundColor: 'white'}}>
                <Image
                    source={{uri: 'https://www.makerguides.com/wp-content/uploads/2023/04/How-To-Connect-ESP32-Bluetooth-With-A-Smartphone-728x410.jpg'}}
                    style={{ width: 300, height: 200, borderRadius: 10, margin: 'auto' }}
                />
            </View>

            <Text style={{marginHorizontal: 10, marginTop: 14, fontFamily: 'Montserrat_400Regular'}}>Ative o bluetooth do seu celular</Text>
            <TextInput placeholder='Nome do dispositivo a se conectar' style={{
                height: 45,
                borderWidth: 1,
                borderColor: '#aaa',
                paddingHorizontal: 12,
                marginHorizontal: 10,
                borderRadius: 10,
                marginBottom: 10,
                fontFamily: 'Montserrat_400Regular'
            }}/>
            <View style={{marginHorizontal: 10}}>
                <Button
                    title="Conectar"
                    color="green"
                />
            </View>

            <Text style={{marginHorizontal: 10, marginVertical: 10, fontFamily: 'Montserrat_400Regular'}}>Dispositivo bluetooth conectado!</Text>
            <TextInput placeholder='Nome da rede wifi' style={{
                height: 45,
                borderWidth: 1,
                borderColor: '#aaa',
                paddingHorizontal: 12,
                marginHorizontal: 10,
                borderRadius: 10,
                marginBottom: 10,
                fontFamily: 'Montserrat_400Regular'
            }}/>
            <TextInput placeholder='Senha da rede wifi' secureTextEntry={true} style={{
                height: 45,
                borderWidth: 1,
                borderColor: '#aaa',
                paddingHorizontal: 12,
                marginHorizontal: 10,
                borderRadius: 10,
                marginBottom: 10,
                fontFamily: 'Montserrat_400Regular'
            }}/>

            <View style={{marginHorizontal: 10}}>
                <Button
                    title="Enviar"
                    color="green"
                />
            </View>
        </View>
    );
}