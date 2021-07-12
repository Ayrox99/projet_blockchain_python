#include "../Bloc.h"
#include "../signature/signature_component/signature_component.h"
#include <cstdlib>
#include "signature_component.h"

using namespace std;
srand((int)time(NULL));

py::object TXI::to_json() const {
	nlohmann::json txi;
	txi["nBloc"] = nBloc;
	txi["nTx"] = nTx;
	txi["nUtxo"] = nUtxo;
	txi["signature"] = signature;
	return txi;
}

py::object UTXO::to_json() const {
	nlohmann::json utxo;
	utxo["nBloc"] = nBloc;
	utxo["nTx"] = nTx;
	utxo["nUtxo"] = nUtxo;
	utxo["montant"] = montant;
	utxo["owner"] = owner;
	return utxo;
}

TX::TX(const nlohmann::json& j) {

	nlohmann::json imputJson = j["imputs"];
	for (nlohmann::json::const_iterator it = imputJson.begin(); it != imputJson.end(); ++it) { //on récupére les TXI des inputs

		nlohmann::json txiJson = j["txi"];
		for (nlohmann::json::const_iterator it1 = txiJson.begin(); it1 != txiJson.end(); ++it1) {
			TXI txi;
			txi.nBloc = *it1["nBloc"];
			txi.nTx = *it1["nTx"];
			txi.nUtxo = *it1["nUtxo"];
			txi.signature = *it1["signature"];
			TXIs.push_back(txi);
		}

		nlohmann::json utxoJson = j["utxo"];
		for (nlohmann::json::const_iterator it1 = utxoJson.begin(); it1 != utxoJson.end(); ++it1) {
			UTXO utxo;
			utxo.nBloc = *it1["nBloc"];
			utxo.nTx = *it1["nTx"];
			utxo.nUTX0 = *it1["nUtxo"];
			utxo.montant = *it1["montant"];
			utxo.owner = *it1["owner"];
			imputUTXOs.push_back(utxo);
		}
	}

	nlohmann::json outputJson = j["outputs"];
	for (nlohmann::json::const_iterator it = outputJson.begin(); it != outputJson.end(); ++it) { //on récupére les couples montant-destinataire
		UTXO utxo;
		utxo.nBloc = *it["nBloc"];
		utxo.nTx = *it["nTx"];
		utxo.nUTX0 = *it["nUtxo"];
		utxo.montant = *it["montant"];
		utxo.owner = *it["owner"];
		outputUTXOs.push_back(utxo);
	}

}

py::object TX::to_json() const {
	nlohmann::json transaction;
	nlohmann::json imputs;
	nlohmann::json txiJson;
	nlohmann::json utxoJson;
	nlohmann::json outputs;
	for (list<TXI>::const_iterator it = TXIs.begin(); it != TXIs.end(); ++it) {
		txiJson.push_back((*it).to_json());
	}
	for (list<UTXO>::const_iterator it = imputUTXOs.begin(); it != imputUTXOs.end(); ++it) {
		utxoJson.push_back((*it).to_json());
	}
	imputs["txi"] = txiJson;
	imputs["utxo"] = utxoJson;
	transaction["imputs"] = imputs;

	for (list<UTXO>::const_iterator it = outputUTXOs.begin(); it != outputUTXOs.end(); ++it) {
		outputs.push_back((*it).to_json());
	}
	transaction["outputs"] = outputs;
	return transaction;
}

list<UTXO> TX::faireTransaction(const nlohmann::json& j)) {

	int montantImputs = 0;
	int montantOutputs = 0;

	nlohmann::json imputJson = j["imputs"];
	for (nlohmann::json::const_iterator it = imputJson.begin(); it != imputJson.end(); ++it) { //on récupére les TXI des inputs
		TXI txi;
		UTXO utxo;

		txi.nBloc = *it["nBloc"];
		utxo.nBloc = *it["nBloc"];

		txi.nTx = *it["nTx"];
		utxo.nTx = *it["nTx"];

		txi.nUtxo = *it["nUtxo"];
		utxo.nUTX0 = *it["nUtxo"];

		txi.signature = ""; 

		utxo.montant = *it["montant"];
		utxo.owner = *it["owner"];

		Signature s;
		txi.signature = s.signMessage(txi.to_json(), utxo.owner);

		TXIs.push_back(txi);
		imputUTXOs.push_back(utxo);

		montantImputs += *it["montant"];
	}

	nlohmann::json outputJson = j["outputs"];
	for (nlohmann::json::const_iterator it = outputJson.begin(); it != outputJson.end(); ++it) { //on récupére les couples montant-destinataire
		UTXO utxo;
		utxo.nBloc = -1;
		utxo.nTx = -1;
		utxo.nUTX0 = rand();
		utxo.montant = *it["montant"];
		utxo.owner = *it["destinataire"];
		outputUTXOs.push_back(utxo);

		montantOutputs += *it["montant"];
	}

	if (montantOutputs > montantImputs) {
		throw runtime_error("Somme des outputs trop grand par rapport aux imputs");
	}

	UTXO utxo;
	utxo.nBloc = -1;
	utxo.nTx = -1;
	utxo.nUTX0 = rand();
	utxo.montant = montantImputs - montantOutputs;
	utxo.owner = ""; //clé du mineur 
	outputUTXOs.push_back(utxo);
}

bool TX::verifierTransaction() {
	Signature s;
	int i = 0;
	for (list<TXI>::const_iterator it1 = TXIs.begin(); it1 != TXIs.end(); ++it1) {
		int j = 0;
		for (list<UTXO>::const_iterator it2 = imputUTXOs.begin(); it2 != imputUTXOs.end(); ++it2) {
			if ( (i == j) && !(s.validateSignature( (*it1).to_json(), (*it2).owner, (*it1).signature) ) ) {
				return false;
			}
			j++;
		}
		i++;
	}
	return true;
	
}

//-------------------------------------------

TXM::TXM(const nlohmann::json& j) {
	nlohmann::json utxoJson = j["utxo"];
	for (nlohmann::json::const_iterator it = utxoJson.begin(); it != utxoJson.end(); ++it) {
		utxo.nBloc = *it["nBloc"];
		utxo.nTx = *it["nTx"];
		utxo.nUTX0 = *it["nUtxo"];
		utxo.montant = *it["montant"];
		utxo.owner = *it["owner"];
	}
}

py::object TX::to_json() const {
	nlohmann::json transaction_coinbase;
	nlohmann::json utxoJson;
	utxoJson.push_back(uxto.to_json());
	transaction_coinbase["utxo"] = utxoJson;
	return transaction_coinbase;
}

PYBIND11_MODULE(transaction_component, m) {
	py::class_<TX>(m, "TX")
		.def(py::init())
		.def(py::init<const nlohmann::json&>())
		.def("to_json", &TX::to_json)
		.def("faireTransaction", &TX::faireTransaction)
		.def("verifierTransaction", &TX::verifierTransaction);
	py::class_<TXM>(m, "TXM")
		.def(py::init())
		.def(py::init<const nlohmann::json&>())
		.def("to_json", &TXM::to_json)
}