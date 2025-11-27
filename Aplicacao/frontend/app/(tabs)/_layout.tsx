import { Ionicons } from '@expo/vector-icons';
import { Tabs } from 'expo-router';
import React from 'react';

export default function TabLayout() {

  return (
    <Tabs
      screenOptions={{
        headerShown: false,
      }}>
      <Tabs.Screen
        name="index"
        options={{
          title: 'Home',
          tabBarIcon: ({ color }) => <Ionicons size={28} name='home' color={color} />,
        }}
      />
      <Tabs.Screen
        name="graficoPassos"
        options={{
          title: 'Gráficos',
          tabBarIcon: ({ color }) => <Ionicons size={28} name='bar-chart' color={color} />,
        }}
      />
      <Tabs.Screen
        name="newDevice"
        options={{
          title: 'Novo Dispositivo',
          tabBarIcon: ({ color }) => <Ionicons size={28} name='wifi-outline' color={color} />,
        }}
      />
    </Tabs>
  );
}