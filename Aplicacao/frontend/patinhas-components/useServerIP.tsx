import { Platform } from 'react-native';

export default function useServerIP(){
  if (Platform.OS === 'ios' || Platform.OS === 'android') {
    // rodando no celular, usa IP dinâmico do servidor
    return "192.168.15.34";
  }
  else{
    // rodando no PC, usa localhost pro servidor
    return "127.0.0.1";
  }

}