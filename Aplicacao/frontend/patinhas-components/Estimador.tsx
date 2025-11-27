/**
 * Estima passos diários "ideais" para um cachorro.
 * Retorna um número (passos/dia) e também um range (min, max) para segurança.
 *
 * Parâmetros:
 *  - pesoKg: number
 *  - porte: 'pequeno'|'medio'|'grande'|'gigante'
 *  - idadeAnos: number
 *  - energia: 'baixa'|'media'|'alta'
 *  - castrado: boolean
 *  - temProblemaSaude: boolean
 *  - objetivo: 'manutencao'|'perda_peso'|'condicionamento'
 *  - ambienteRuim: boolean (clima/terreno)
 */

export function estimarPassosDiarios(animal:any) {
  let pet:any = animal.pet;

  if (!pet.pesoKg || pet.pesoKg <= 0) return { estimate: 0, min: 0, max: 0 };

  const baseFactors:any = {
    pequeno: 120,
    medio: 140,
    grande: 160,
    gigante: 170,
  };
  const baseFactor = baseFactors[pet.porte] ?? baseFactors.medio;

  // base por peso
  const base = pet.pesoKg * baseFactor;

  // modifiers
  const ageModifier = pet.idadeAnos < 1 ? 0.9 : (pet.idadeAnos > 7 ? 0.7 : 1.0);

  const energyModifier = pet.energia === 'baixa' ? 0.8 : pet.energia === 'alta' ? 1.25 : 1.0;
  const neuteredModifier = pet.castrado ? 0.95 : 1.0;
  const healthModifier = pet.temProblemaSaude ? 0.75 : 1.0;

  const goalModifier = pet.objetivo === 'perda_peso' ? 1.25 : pet.objetivo === 'condicionamento' ? 1.15 : 1.0;
  const envModifier = pet.ambienteRuim ? 0.8 : 1.0;

  let estimate = base
    * ageModifier
    * energyModifier
    * neuteredModifier
    * healthModifier
    * goalModifier
    * envModifier;

  // Dar um range de segurança ±15%
  const min = Math.round(estimate * 0.85);
  const max = Math.round(estimate * 1.15);
  estimate = Math.round(estimate);

  return { estimate, min, max };
}

export function calcularMediaPassos(animal:any, ultimosDias: any) {
  let pet:any = animal.pet;
  if (!pet || !pet.passos || pet.passos.length === 0) return 0;

  const hoje:any = new Date();

  // Média da vida inteira
  if (!ultimosDias) {
    const soma = pet.passos.reduce((total: any, p: { num_passos: any; }) => total + p.num_passos, 0);
    return soma / pet.passos.length;
  }

  // Média dos últimos X dias
  const passosFiltrados = pet.passos.filter((p:any) => {
    const dataPassos:any = new Date(p.data);
    const diffDias = (hoje - dataPassos) / (1000 * 60 * 60 * 24);
    return diffDias <= ultimosDias;
  });

  if (passosFiltrados.length === 0) return 0;

  const soma = passosFiltrados.reduce((total: any, p: { num_passos: any; }) => total + p.num_passos, 0);

  return soma / passosFiltrados.length;
}