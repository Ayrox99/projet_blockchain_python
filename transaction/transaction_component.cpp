#include "../Bloc.h"
#include <vector>
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
	utxo["hash"] = hash;
	return utxo;
}

TX::TX(const nlohmann::json& j) {

	nlohmann::json imputJson = j["imputs"];
	for (nlohmann::json::const_iterator it = imputJson.begin(); it != imputJson.end(); ++it) { //on récupére les TXI des inputs

		nlohmann::json txiJson = j["txi"];
		for (nlohmann::json::const_iterator it1 = txiJson.begin(); it1 != txiJson.end(); ++it) {
			TXI txi = new TXI();
			txi.nBloc = *it1["nBloc"];
			txi.nTx = *it1["nTx"];
			txi.nUtxo = *it1["nUtxo"];
			txi.signature = *it1["signature"];
			TXIs.push_back(txi);
		}

		nlohmann::json utxoJson = j["utxo"];
		for (nlohmann::json::const_iterator it1 = utxoJson.begin(); it1 != utxoJson.end(); ++it) {
			TXI utxo = new UTXO();
			utxo.nBloc = *it1["nBloc"];
			utxo.nTx = *it1["nTx"];
			utxo.nUTX0 = *it1["nUtxo"];
			utxo.montant = *it1["montant"];
			utxo.owner = *it1["owner"];
			utxo.hash = *it1["hash"];
			imputUTXOs.push_back(utxo);
		}
	}

	nlohmann::json outputJson = j["outputs"];
	for (nlohmann::json::const_iterator it = outputJson.begin(); it != outputJson.end(); ++it) { //on récupére les couples montant-destinataire
		TXI utxo = new UTXO();
		utxo.nBloc = *it["nBloc"];
		utxo.nTx = *it["nTx"];
		utxo.nUTX0 = *it["nUtxo"];
		utxo.montant = *it["montant"];
		utxo.owner = *it["destinataire"];
		utxo.hash = *it["hash"];
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
		TXI txi = new TXI();
		TXI utxo = new UTXO();

		txi.nBloc = *it["nBloc"];
		utxo.nBloc = *it["nBloc"];

		txi.nTx = *it["nTx"];
		utxo.nTx = *it["nTx"];

		txi.nUtxo = *it["nUtxo"];
		utxo.nUTX0 = *it["nUtxo"];

		txi.signature = *it["signature"]; //TODO vérifier

		utxo.montant = *it["montant"];
		utxo.owner = *it["owner"];
		utxo.hash = *it["hash"]; //TODO vérifier

		TXIs.push_back(txi);
		imputUTXOs.push_back(utxo);

		montantImputs += *it["montant"];
	}

	nlohmann::json outputJson = j["outputs"];
	for (nlohmann::json::const_iterator it = outputJson.begin(); it != outputJson.end(); ++it) { //on récupére les couples montant-destinataire
		TXI utxo = new UTXO();
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

	TXI utxo = new UTXO();
	utxo.nBloc = -1;
	utxo.nTx = -1;
	utxo.nUTX0 = rand();
	utxo.montant = montantImputs - montantOutputs;
	//utxo.owner = ;//clé du mineur 
	outputUTXOs.push_back(utxo);
}

bool TX::verifierTransaction() {
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
		utxo.hash = *it["hash"];
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