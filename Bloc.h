#pragma once
#ifndef _BLOC_H

#include <string>
#include <list>
#include <pybind11/pybind11.h>
#include <nlohmann/json.hpp>
#include <pybind11_json/pybind11_json.hpp>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
using namespace std;
namespace py = pybind11;
//
// bloc.h  version 1.3
//
// projet blockchain M2IF 2018-2019-2020-2021
//

#define KEY_SIZE 64 // taille de la chaine hexadecimape de la cle privee
#define PUBLIC_KEY_SIZE 128
#define HASH_SIZE 64  // taille du hash, 256 bits apres conversion en chaine hexadecimale
#define SIGNATURE_SIZE 128

class TXI // input d'une transaction, correspondant a la depense d'une UTXO
{
    // principe: on reference l'UTXO que l'on est en train de depenser
    // cette reference est signee, on verifie en utilisant la cle publique dans l'UTXO
    // que cette TXI a bien ete signee par la cle public, donc par son proprietaire.
 public:
	unsigned int nBloc; // numero de bloc de l'UTXO correspondant a  cette TXI
	unsigned int nTx;   // numero de la transaction de TX dans le bloc
	unsigned int nUtxo; // numero de l'UTXO dans la transaction TX
	string signature[128]; // signature des 3 champs precedents, verifiable
	py::object to_json() const;
};

class UTXO // une sortie non d�pens�e
{
 public:
    // les 3 champs suivants nBloc,nTx,nUTX0) forment l'identifiant unique de l'UTXO
    int nBloc;        // numero du bloc dans lequel est inclu cette TXO
    int nTx;          // numero de la transaction dans le bloc dans laquelle est inclu cette TXO
    int nUTX0;        // numero de l'UTXO dans lla transaction
    int montant;      // montant de la transaction
  unsigned char owner[PUBLIC_KEY_SIZE];  //	cl� publique du compte destinataire (le nouveau proprietaire)
  unsigned char hash[HASH_SIZE];    // hash(nBloc,nTx,nUTXO,montant,destinataire) pour securisation de l'UTXO
  py::object to_json() const;

};

class TX { // transaction standard (many inputs, many outputs)
 public:
  TX() {}; //transaction standard exemple
  TX(const nlohmann::json &j){};
  py::object to_json() const{
      nlohmann::json j;
      j["name"]="dummy";
      return j;
  };
  list<UTXO> faireTransaction(const nlohmann::json& j));
  bool verifierTransaction();

  list<TXI>	TXIs;
  list<UTXO> imputUTXOs;
  list<UTXO> outputUTXOs;
};

class TXM { // transaction du mineur : coinbase
 public:
    TXM(){};
    TXM(const nlohmann::json &j){};
    py::object to_json() const{
	nlohmann::json j;
	return j;
    };
    UTXO utxo;
};

class Bloc
{
 public :
	Bloc(); // cree un bloc exemple
	Bloc(const nlohmann::json &j);
	py::object to_json() const;
 public:
	string hash; // hash des autres champs, hash of the entire bloc
	unsigned int nonce;

	string previous_hash; // hash of the previous bloc
	int num; // numero du bloc, commence a zero
	list<TX> txs; //  transactions du bloc
	TXM tx0; // transaction du mineur (coinbase)

	void setNonce(int);
	unsigned int getNonce();

};



#endif
