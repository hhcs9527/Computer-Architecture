#include <iostream>
#include <fstream>
#include <cmath>
#include <math.h>
#include <vector>



using namespace std;

// For checking hit/miss, replace or not
struct Hit_Replace{
    bool Hit;
    bool Replace;
};

// For file writing result 
struct Result{
    vector <int> Index_bits;
    vector <string> Prediction;
    int Cache_miss;
};

// Read Cache data
void read_cache(int &add_bit, int &block_size, int &cache_size, int &associativity, string Cache_org)
{
    string S;
    ifstream fin(Cache_org.c_str());
    fin >> S >> add_bit;
    fin >> S >> block_size;
    fin >> S >> cache_size;
    fin >> S >> associativity;
    fin.close();
}


void read_index_record(vector <string> &history, string index_history)
{
    string S;
    ifstream fin(index_history.c_str());
    while(fin >> S){
        history.push_back(S);
    } 
    fin.close();
}

void get_LSB_index(vector <int> &LSB_index, int Indexing_bit_count, int Tag_bit_count){
    int i;
    for (i = 0; i < Indexing_bit_count; i++){
        LSB_index.push_back(Tag_bit_count + i);
    }
}


// for calculating Correlation
float C(vector< vector<int> > index, int i, int j){
    int E = 0, D = 0, l, length_of_index_history = index.at(0).size();
    float Cij;
    for (l = 0; l < length_of_index_history; l++){
        if (index.at(i).at(l) == index.at(j).at(l)){
            E++;
        }    
    }
    
    D = length_of_index_history - E;
    Cij = (float)min(E,D)/max(E,D);
    return Cij;
}

// Prepare for Zero_cost
void prepare(vector<string> index_history, vector<float> &Q, vector< vector<float> > &Correlation, vector<int> &AtSet)
{
    // index_history.at(0).length()-2 for 換行字元
    int i, j, number_of_address_bit = Q.size(), length_of_index_history = index_history.size();
    
    int number_of_zero = 0, number_of_nonzero = 0;
    vector< vector<int> > index(Q.size());
    // for index
    for(i = 2; i < length_of_index_history-1; i++){
        // access  0,1,2,3,4,5,6,7 (in program)
        // address 7,6,5,4,3,2,1,0 (in Cache)
        for (j = 0; j < number_of_address_bit; j++){
            index.at(j).push_back(index_history.at(i).at(j) - '0');
        }
    }

    // for Q
    for(i = 0; i < number_of_address_bit; i++){
        number_of_zero = 0;
        number_of_nonzero = 0;
        int sumv = 0;
        for (j = 2; j < length_of_index_history - 1; j++){
            if (index.at(i).at(j-2) == 0){number_of_zero++;}
            else{number_of_nonzero++;}
            sumv += index.at(i).at(j-2);
        }
        Q.at(i) = (float)min(number_of_nonzero, number_of_zero)/max(number_of_nonzero, number_of_zero);
    }
    
    // for Correlation
    for(i = 0; i < number_of_address_bit; i++){
        AtSet.at(i) = 0;
        for (j = 0; j < number_of_address_bit; j++){
            Correlation.at(i).push_back(C(index, i, j));
        }
    }    
}

// find the max of Q & push it into S
int find_maxQ(vector<float> &Q, vector<int> &S, vector<int> &AtSet, int Offset_bit_count){
    int i;
    float maxA = -1;
    int maxindex = 0;
    for(i = 0; i < Q.size(); i++){
        if(Q.at(i) > maxA && AtSet.at(i) == 0){
            // Mean address bit
            maxA = Q.at(i);
            maxindex = i;
        }
    }
    S.push_back(maxindex);
    AtSet.at(maxindex) = 1; 
    return maxindex;
}

// Zero_cost
void Zero_cost(vector<float> &Q, vector< vector<float> > Correlation, vector<int> &S, vector<int> &AtSet, int Indexing_bit_count, int Offset_bit_count)
{
    int i, j, best, address_bit = Correlation.size();   
    float tmp;
    for (i = 0 ; i < Indexing_bit_count; i++){
        // find the max of Q & push it into S & update Atset
        best = find_maxQ(Q, S, AtSet, Offset_bit_count);
        for (j =0; j < address_bit; j++){
            tmp = (float)Q.at(j) * Correlation.at(best).at(j);
            Q.at(j) = tmp;
        }
    }
}

// access  0,1,2,3,4,5,6,7 (in program)
// address 7,6,5,4,3,2,1,0 (in Cache)
void Reset_Addressing_bits(vector<int> &Addressing_bits, int Offset_bit_count){
    int i;
    // for non offset bit
    for (i = 0; i < Addressing_bits.size()-Offset_bit_count; i++){
        Addressing_bits.at(i) = 0;
    }
    // for offset bit
    for (i = Addressing_bits.size()-Offset_bit_count; i < Addressing_bits.size(); i++){
        Addressing_bits.at(i) = 1;
    }
}


// LSB
// storing access mode
void get_index_Tag(vector<int> &Indexing_bits, vector<int> &Tag_bits, int Offset_bit_count, int Indexing_bit_count, int Tag_bit_count, int Address_bits, 
                   int isZero, vector <int> &Addressing_bits, vector<int> Zero_index, vector <int> LSB_index)
{
    // access  0,1,2,3,4,5,6,7 (in program)
    // address 7,6,5,4,3,2,1,0 (in Cache)

    int i, j;
    // LSB
    if (isZero == 0){
        // Setting Index
        for (i = 0; i < Indexing_bits.size(); i++){
            Indexing_bits.at(i) = LSB_index.at(i);
            Addressing_bits.at(LSB_index.at(i)) = 1;
            //cout << Indexing_bits.at(i) << " ";
        }

        // Setting Tag
        j = 0;
        for (i = 0; i < Addressing_bits.size(); i++){
            if(Addressing_bits.at(i) == 0){
                Tag_bits.at(j) = i;
                j++;
            }
        }
    }
    else{
        // Setting Index
        for (i = 0; i < Indexing_bits.size(); i++){
            Indexing_bits.at(i) = Zero_index.at(i);
            Addressing_bits.at(Zero_index.at(i)) = 1;
        }


        // Setting Tag
        j = 0;
        for (i = 0; i < Addressing_bits.size(); i++){
            if(Addressing_bits.at(i) == 0){
                Tag_bits.at(j) = i;
                j++;         
            }
        }
    }
}

void Setup_Essential( vector< vector<int> > &Occupy, vector< vector<string> > &Tag, vector< vector<int> > &NRU, 
        vector<int>  &NRU_counter, int Cache_sets, int Associativity)
{   
    int i, j;
    for (i = 0; i < Cache_sets; i++){
        for (j = 0; j < Associativity; j ++){
            // 0 for non, 1 for occupy
            Occupy[i].push_back(0);
            Tag[i].push_back("");
            NRU[i].push_back(1);
        }
        NRU_counter.at(i) = Associativity;
    }
}

void Reset_Essential( vector< vector<int> > &Occupy, vector< vector<string> > &Tag, vector< vector<int> > &NRU,
        vector<int>  &NRU_counter, int Cache_sets, int Associativity)
{
    int i, j;
    for (i = 0; i < Cache_sets; i++){
        for (j = 0; j < Associativity; j ++){
            // 0 for non, 1 for occupy
            Occupy.at(i).at(j) = 0;
            Tag.at(i).at(j) = "";
            NRU.at(i).at(j) = 1;
        }
        NRU_counter.at(i) = Associativity;
    }
}



int find_set(vector<int> Indexing_bits, int Indexing_bit_count, string index){
    int i, set = 0, count = Indexing_bit_count - 1; // Indexing_bit_count = 2, max index only 3
    for (i = 0; i < Indexing_bit_count;  i++){
        set += (int)pow (2.0, count) * (index[Indexing_bits[i]] - '0');
        count --;
    }
    return set;
}

void get_reference_Tag(string &reference_Tag, vector<int> &Tag_bits, string index_history){
    int i;
    string Tag = "";
    for (i = 0; i < Tag_bits.size();  i++){
        Tag += index_history.at(Tag_bits.at(i));
    }
    reference_Tag = Tag;
}


Hit_Replace Check_cache(vector<int> &Occupy, vector<string> &Tag, vector<int> &NRU, int &NRU_counter
                        , string reference_Tag)
{
    Hit_Replace HR;
    int i, index;
    bool empty = false; // true -> there is empty block, false otherwise
    for(i = 0 ; i < Occupy.size(); i++){
        if (Occupy.at(i) == 1){
            if (Tag.at(i) == reference_Tag){
                HR.Hit = true;
                HR.Replace = false;
                // if not hit before consuder repeat hit
                if (NRU.at(i) != 0){
                    NRU_counter--;
                }
                NRU.at(i) = 0;
                return HR;
            }
            else{
                HR.Hit = false;
                HR.Replace = true;                
            }
        }   
        // Ｎot been occupied
        else{
            empty = true;
            index = i;
            HR.Hit = false;
            HR.Replace = false;
        }
    }
    // Miss But with empty
    if (empty)
    {
        HR.Replace = false;
        Occupy.at(index) = 1;
        Tag.at(index) = reference_Tag;
        NRU.at(index) = 0;
        NRU_counter --;
    }
    return HR;
}



void Cache_Replacement(vector<string> &Tag, vector<int> &NRU, int &NRU_counter, string reference_Tag, int Associativity)
{   
    int i; 
    // In this Set
    if (NRU_counter == 0){
        for(i = 0; i < NRU.size(); i++) {
            NRU.at(i) = 1;
        }
        NRU_counter = Associativity;
    }

    // find victim index 
    int victim;
    for(i = 0; i < NRU.size(); i++){
        // since the head should be the left, but our assignment is the right, so our head is in the right
        // which is the last one where NRU.at(i) == 1
        if (NRU.at(i) == 1){
            victim = i;
        }
    }
    // Replace the victim with reference_Tag and update
    Tag.at(victim) = reference_Tag;
    NRU.at(victim) = 0;
    NRU_counter --;
}

void update_predict_result(vector <string> &Zero_result, vector <string> &LSB_result, Hit_Replace hr, int &Total_cache_miss_count, int mode){
    if (mode){
        if (hr.Hit) {Zero_result.push_back("hit");}
        else{
            Zero_result.push_back("miss");
            Total_cache_miss_count ++;
        }
    }

    else
    {
        if (hr.Hit) {LSB_result.push_back("hit");}
        else{
            LSB_result.push_back("miss");
            Total_cache_miss_count ++;
        }
    }
}

void File_writing(int Address_bits, int Block_size, int Cache_sets, int Associativity, int Offset_bit_count, int Indexing_bit_count, 
                 vector <string> index_history, Result Res, string output_file)
{
    ofstream myfile (output_file.c_str());
    int i;
    if (myfile.is_open())
    {
        myfile << "Address bits: " << Address_bits << endl;
        myfile << "Block size: " << Block_size << endl;
        myfile << "Cache sets: " << Cache_sets << endl;
        myfile << "Associativity: " << Associativity << endl << endl; 
        myfile << "Offset bit count: " << Offset_bit_count << endl;
        myfile << "Indexing bit count: " << Indexing_bit_count << endl;
        myfile <<  "Indexing bits: ";
        // access  0,1,2,3,4,5,6,7 (in program)
        // address 7,6,5,4,3,2,1,0 (in Cache)
        // show cache mode
        for (i = 0; i < Res.Index_bits.size(); i++)
        {
            if (i+1 == Res.Index_bits.size()){myfile << Address_bits -1 - Res.Index_bits.at(i) << endl; }
            else{myfile << Address_bits -1 - Res.Index_bits.at(i) << " "; }
        }
        
        myfile  << endl << index_history.at(0) << " " << index_history.at(1) << endl;
        for (i = 0; i < Res.Prediction.size(); i++)
        {
            myfile << index_history.at(i+2) << " " << Res.Prediction.at(i) << endl;
        }        
        myfile << index_history.at(index_history.size()-1) << endl << endl;
        myfile << "Total cache miss count: " << Res.Cache_miss << endl << endl;
        myfile.close();
    }
}



int main(int argc, char *argv[])
{   
    string Cache_org, Reference_list, output_file;
    Cache_org = (string)argv[1];
    Reference_list = (string)argv[2];
    output_file = (string)argv[3];

    // for for loop, Total cache miss count
    int i, j, Total_cache_miss_count;

    // Read cache spec
    int Address_bits, Block_size, Cache_sets, Associativity;
    read_cache(Address_bits, Block_size, Cache_sets, Associativity, Cache_org);

    // Calculate info
    int Offset_bit_count, Indexing_bit_count, Tag_bit_count;
    Offset_bit_count = int(log2f(Block_size));
    Indexing_bit_count = int(log2f(Cache_sets));
    Tag_bit_count = Address_bits - Offset_bit_count - Indexing_bit_count;
    // for recording which bit have not been used (0 non, 1 used)
    // in access mode
    vector <int> Addressing_bits(Address_bits);
    

    // Read index record
    vector<string> index_history;
    read_index_record(index_history, Reference_list);

    
    // May be for LSB / Zero_cost indexing mode
    // LSB for access mode
    vector <int> LSB_index;
    get_LSB_index(LSB_index, Indexing_bit_count, Tag_bit_count);
    // Saving the redult of it
    Result LSB_result;
    

    // Zero_cost need extra information
    vector<float>  Q(Address_bits - Offset_bit_count);
    vector< vector<float> > Correlation(Address_bits - Offset_bit_count);
    // if bit at set - >1 , otherwise - > 0
    vector<int> AtSet(Address_bits - Offset_bit_count);
    // result of sub optimal indexing, start from Zero_index.at(0)
    // access mode
    vector<int> Zero_index; 
    // Saving the redult of it
    Result Zero_result;
    prepare(index_history, Q, Correlation, AtSet);
    Zero_cost(Q, Correlation, Zero_index, AtSet, Indexing_bit_count, Offset_bit_count);

    // Create needed vector, size == whole cache size
    // For the cache block been occupied or not (0 for non, 1 for occupy)
    vector< vector<int> >  Occupy(Cache_sets);
    // For Tag in each cache block
    vector< vector<string> >  Tag(Cache_sets);
    // For NRU bits either 1 or 0
    vector< vector<int> >  NRU(Cache_sets);
    // NRU_counter for number of NRU set
    vector<int>  NRU_counter(Cache_sets);
    Setup_Essential(Occupy, Tag, NRU, NRU_counter, Cache_sets, Associativity);

    // Checking Set
    int Set;
    // for checking with the tag in cache
    string reference_Tag;
    // Recording if the data hit/miss and replace or not
    Hit_Replace hr;

    // Get index_bit, Tag_bit in access mode (different indexing method, LSB, zero_cost here)
    vector<int> Indexing_bits(Indexing_bit_count), Tag_bits(Tag_bit_count);

    
    int mode, isZero;
    for (mode = 0; mode < 2; mode++){
        // if indexing method is zero bit;
        // 0 -> no, 1 -> yes
        isZero = mode;
        Total_cache_miss_count = 0;
        // Reset the Addressing bits
        Reset_Addressing_bits(Addressing_bits, Offset_bit_count);
        get_index_Tag(Indexing_bits, Tag_bits, Offset_bit_count, Indexing_bit_count, Tag_bit_count, Address_bits, isZero, Addressing_bits, Zero_index, LSB_index);

        Reset_Essential(Occupy, Tag, NRU, NRU_counter, Cache_sets, Associativity);


        // Do reference and Cache Replacement
        for (i = 2 ; i < index_history.size()-1 ; i++)
        {   
  
            // Decide which set in cache to look up 
            Set = find_set(Indexing_bits, Indexing_bit_count, index_history.at(i));


            // get reference_Tag and check_ache return hit/other
            get_reference_Tag(reference_Tag, Tag_bits, index_history.at(i));
            
            // check if Hit, so we must pass a Tag to check
            // Update the content of Occupy.at(Set), Tag.at(Set), NRU.at(Set), reference.at(Set), NRU_counter, reference_counter
            hr = Check_cache(Occupy.at(Set), Tag.at(Set), NRU.at(Set), NRU_counter.at(Set), reference_Tag);
            
            // Append Hit / Miss to predict_result
            update_predict_result(Zero_result.Prediction, LSB_result.Prediction, hr, Total_cache_miss_count, mode);
            
            // Since we have to replace to go on, do cache replacement algorithm
            if (hr.Replace == true){
                Cache_Replacement(Tag.at(Set), NRU.at(Set), NRU_counter.at(Set), reference_Tag, Associativity);
            }
        }      
        
        
        // LSB
        if (mode == 0){
            LSB_result.Index_bits = Indexing_bits;
            LSB_result.Cache_miss = Total_cache_miss_count;
        }
        else
        {
            Zero_result.Index_bits = Indexing_bits;
            Zero_result.Cache_miss = Total_cache_miss_count;
        }
    }
    if (Zero_result.Cache_miss < LSB_result.Cache_miss){
        // write file with a better result
        File_writing(Address_bits, Block_size, Cache_sets, Associativity, Offset_bit_count, Indexing_bit_count, 
                    index_history, Zero_result, output_file);
    }
    else{
        // write file with a better result
        File_writing(Address_bits, Block_size, Cache_sets, Associativity, Offset_bit_count, Indexing_bit_count, 
                    index_history, LSB_result, output_file);
    }

    return 0;
}
