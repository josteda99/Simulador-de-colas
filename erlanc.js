const betaArrivla = 8.7;
const betaDeparture = 6.1;
const lambda = 1 / betaArrivla;
const mu = 1 / betaDeparture;
const servers = 5;

const a = 1 / factorial(servers);
const b = (lambda / mu) ** servers;
const ro = lambda / (servers * mu);
const factor = 1 / (1 - ro);

let sum = 0;
for (var n = 1; n < servers; n++) {
  sum += (1 / factorial(n)) * ro ** n;
}

const erlang = (a * b * ro) / (1 + sum + a * b * ro);

console.log("Erlang Nuestro: ", erlang);

//functions
function factorial(n) {
  var total = 1;
  for (i = 1; i <= n; i++) {
    total = total * i;
  }
  return total;
}
