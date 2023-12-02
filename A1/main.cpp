#include "FP_Tree_handler.hpp"
#include "fptree.hpp"

#define FOR(i, a, b) for(int i = a; i < b; i ++)

using namespace std;

int max_item_no = 0;
int max_pattern_size = 0;
int max_patterns_to_be_considered = 1e9;
int max_checks = 1e8;
float avg_trans_length,num_of_items,num_of_transactions; 
map<int,int>m, minv;
vector<pair<int,int>>v;

bool cmp(pair<int, int>& a,
        pair<int, int>& b)
{
    return a.second > b.second;
}

/*
Functions implemented : 
1. vector<vector<int>>* read_file_into_transactions(string file_name)
2. void compress_transactions(vector<vector<int>> &transactions_orig, vector<vector<int>> &patterns, int &max_item_no)
3. void print_output(vector<vector<int>> &v, string outfile)
4. void print_pattern_map(map<int, set<int>> &inverse_pattern_map, string pattern_map_file)
5. void print_compression_ratio(vector<vector<int>> &transaction_comp, vector<vector<int>> &transactions_orig, map<int, set<int>> &inverse_pattern_map, set<int> patterns_used)
6. void decompress(string out_file_name, map<set<int>, int> &pattern_map, string, string decompressed_file)
*/

// write the output in the output file
void print_output(vector<vector<int>> &v, string outfile)
{
    ofstream outputFile;
    outputFile.open(outfile, std::ios::app);
    for(auto i: v)
    {
        for(auto j:i)
        {
            outputFile<<j<<" ";
        }
        outputFile<<"\n";
    }
    outputFile.close();
}


//reading the input file and making the transactions
vector<vector<int>>* read_file_into_transactions(string file_name)
{
    std::ifstream myfile(file_name);
    vector<vector<int>>* transactions = new vector<vector<int>>;
    std::string line;

    int tr_id = 0;
    float sum=0;
    set<int>t;

    if (myfile.is_open()) 
    {
        int index=1;
        while (getline(myfile, line)) 
        {
            vector<int> curr_transaction;
            std::istringstream iss(line);
            int num;
            while (iss >> num) 
            {
                max_item_no = max(max_item_no, num);
                curr_transaction.push_back(num);
                t.insert(num);

                m[num] ++;

                // if(m.find(num) == m.end())
                // {
                //     m[num] = index;
                //     index++;
                //     v.push_back({num,1});
                // }

                // else{
                //     v[m[num]].second++;
                // }
            }
            sum+=curr_transaction.size();
            transactions->push_back(curr_transaction);
        }

        myfile.close();
    }
    avg_trans_length=sum/transactions->size();
    num_of_items=t.size();
    num_of_transactions=transactions->size();
    cout<<"avg transaction length IS : "<<avg_trans_length<<"\n";
    cout<<"no of transactions IS : "<<transactions->size()<<"\n";
    cout<<"no of items IS : "<<t.size()<<"\n";
    //cout<<"sparseness in percent IS : "<<((avg_trans_length*transactions->size())*100.0/t.size())/transactions->size()<<"\n";
    return transactions;
}

/*
Output the compression ratio which is calculated as :
(No. of items in compressed transactions +  size of map)/No. of items in original transactions
*/
void print_compression_ratio(vector<vector<int>> &transaction_comp, vector<vector<int>> &transactions_orig, map<int, set<int>> &inverse_pattern_map, set<int> &patterns_used)
{   
    int size_of_transactions = 0;
    int size_of_comprtransactions = 0;
    int map_size = 0;

    for(auto transaction : transactions_orig)
    {
        size_of_transactions += transaction.size();
    }

    for(auto transaction : transaction_comp)
    {
        size_of_comprtransactions += transaction.size();
    }

    for(auto x: patterns_used)
    {
        if(inverse_pattern_map[x].size() > 1)
        {
            map_size += inverse_pattern_map[x].size() + 1;
        }
    }

    cout<<"MAP SIZE : "<<map_size<<"\n";
    cout<<"COMPRESSED TRANS SIZE : "<<size_of_comprtransactions<<"\n";
    cout<<"ORIG TRANS SIZE : "<<size_of_transactions<<"\n";
    float compression_ratio = (float(size_of_comprtransactions + map_size)*100/size_of_transactions);
    cout<<"COMPRESSION RATIO IS : "<<compression_ratio<<"\n";
}

// Outputing the pattern_map as a file which will be used for decompression later on
void print_pattern_map(map<int, set<int>> &inverse_pattern_map, string pattern_map_file, set<int> &patterns_used)
{
    ofstream outfile;
    outfile.open(pattern_map_file);
    cout<<"NO OF PATTERNS USED : "<<patterns_used.size()<<"\n";

    for(auto x: patterns_used)
    {
        if(inverse_pattern_map[x].size() > 1)
        {
            outfile<<x<<" ";
            for(auto item : inverse_pattern_map[x])
            {
                outfile<<item<<" ";
            }
            outfile<<"\n";
        }
    }
    outfile<<"\n";
    outfile.close();
}

// Outputting the final decompressed file using the pattern map and compressed file

void decompress(string compressed_file_name, string decompressed_file){
    map<int, set<int>>pattern_map_inverse;

    std::ifstream myfile(compressed_file_name);
    std::ofstream outfile;
    outfile.open(decompressed_file);

    std::string line;

    std::streampos resumePosition = 0;

    if (myfile.is_open())
    {
        while (getline(myfile, line))
        {
            if (line.empty()) {
                // Blank line encountered, remember the current position
                resumePosition = myfile.tellg();
                break; 
            }
            std::istringstream iss(line);
            int num;
            iss>>num;
            int temp = num;
            while (iss >> num)
            {
                pattern_map_inverse[temp].insert(num);
            }
        }
    }

    cout<<"Pattern map size : "<<pattern_map_inverse.size()<<"\n";

    // myfile.clear();  // Clear any error flags
    myfile.seekg(resumePosition); 

    while (getline(myfile, line))
    {
        std::istringstream iss(line);
        int num;
        vector<int> v;

        while (iss >> num)
        {
            if(pattern_map_inverse[num].size()>0){
                for(auto z : pattern_map_inverse[num]){
                    v.push_back(z);
                }
            }
            
            else
            // outfile << num << " ";
            v.push_back(num);

        }
        // outfile <<"\n";
        sort(v.begin(),v.end());
        for(auto elem : v){
            outfile<<elem<<" ";
        }
        outfile<<"\n";
    }

    outfile.close();

}

// Given the original transactions and the identified frequent itemsets, outputs the compressed transactions
void compress_transactions(vector<vector<int>> &transactions_orig, vector<vector<int>> &patterns, int &max_item_no, string outfilename)
{
    cout<<"MAX ITEM NO : "<<max_item_no<<"\n";
    auto start_comp = chrono::high_resolution_clock::now();
    vector<vector<int>> transaction_comp(transactions_orig.size(), vector<int>(0,0));
    map<int, int> item_frequencies;
    map<set<int>, int> pattern_map;
    map<int, set<int>> inverse_pattern_map;
    set<int> patterns_used;

    int current_adder = 1;
    int transaction_id = 0;
    int curr_item = 0;
    int p_size = 0;
    //storing the no of patterns in which each element occurs + assigning keys to patterns and inserting them into a map
    // cout<<"COMPRESSION a =====================================================================\n";
    // vector<pair<int, int>> temp;
    // FOR(i,0,patterns.size())
    // {
    //     temp.push_back({patterns[i].size(), i});
    // }
    // // sort(temp.begin(), temp.end());
    // cout<<"PATTERNS SORTED=====================================================================\n";

    
    // int pattern_id = 0;
    // int temp_size = temp.size();
    // FOR(temp_id, 0, min(max_patterns_to_be_considered, temp_size))
    // {   
    //     pattern_id = temp[temp_id].second;
    //     if(patterns[pattern_id].size() > 1)
    //     {
    //         // cout<<"Pattern size : "<<patterns[pattern_id].size()<<"\n";
    //         p_size = patterns[pattern_id].size();
    //         max_pattern_size = max(max_pattern_size, p_size);
    //         set<int> current_set;
    //         FOR(i, 0, patterns[pattern_id].size())
    //         {
    //             curr_item = patterns[pattern_id][i];
    //             item_frequencies[curr_item] ++;
    //             current_set.insert(curr_item);
    //             // cout<<"hello\n";
    //         }
    //         pattern_map[current_set] = max_item_no + current_adder;
    //         inverse_pattern_map[max_item_no + current_adder] = current_set;
    //         current_adder ++;
    //     }
    //     // cout<<current_adder<<" - assigned\n";
    // }

    FOR(pattern_id, 0, patterns.size())
    {   
        if(patterns[pattern_id].size() > 1)
        {
            p_size = patterns[pattern_id].size();
            max_pattern_size = max(max_pattern_size, p_size);
            set<int> current_set;
            FOR(i, 0, patterns[pattern_id].size())
            {
                curr_item = patterns[pattern_id][i];
                item_frequencies[curr_item] ++;
                current_set.insert(curr_item);
                // cout<<"hello\n";
            }
            pattern_map[current_set] = max_item_no + current_adder;
            inverse_pattern_map[max_item_no + current_adder] = current_set;
            current_adder ++;
        }
        // cout<<current_adder<<" - assigned\n";
    }


    cout<<"NO. OF PATTERNS : "<<pattern_map.size()<<"\n";
    // cout<<"COMPRESSION b =====================================================================\n";

    //compressing for each transaction individually
    for(auto transaction : transactions_orig)
    {   
        //sorting the transaction items based on the no of patterns in which they appear
        vector<pair<int, int>> curr_transaction_counts;
        for(auto curr_item : transaction)
        {
            curr_transaction_counts.push_back({item_frequencies[curr_item], curr_item});
        }
        sort(curr_transaction_counts.begin(), curr_transaction_counts.end());

        //processing the transaction greedily
        vector<bool> item_compressed(curr_transaction_counts.size(), false);

        for(int item_id = 0; item_id < curr_transaction_counts.size(); item_id++)
        {
            //if this element has not been seen yet
            if(item_compressed[item_id] == false)
            {
                item_compressed[item_id] = true;
                int current_key = curr_transaction_counts[item_id].second;
                set<int> final_set = {curr_transaction_counts[item_id].second};
            
                //find the best frequent itemset to cover this particular item
                if(curr_transaction_counts[item_id].first > 0)
                {
                    for(int next_item_id = item_id+1; next_item_id < curr_transaction_counts.size(); next_item_id++)
                    {
                    //if this prospective element has not been seen yet
                        if(curr_transaction_counts[next_item_id].first > 0 && item_compressed[next_item_id] == false)
                        {
                            final_set.insert(curr_transaction_counts[next_item_id].second);
                            //if we have a frequent itemset
                            if(pattern_map[final_set] != 0)
                            {
                                current_key = pattern_map[final_set];
                                item_compressed[next_item_id] = true;
                                if(final_set.size() == max_pattern_size)
                                {
                                    break;
                                }
                            }
                            //if we dont have a frequent itemset
                            else
                            {
                                final_set.erase(final_set.find(curr_transaction_counts[next_item_id].second));
                            }
                        }
                    }
                }
                
                if(curr_transaction_counts[item_id].second != current_key)
                {
                    patterns_used.insert(current_key);
                }
                transaction_comp[transaction_id].push_back(current_key);
            }
        }
        transaction_id++;
        if(transaction_id % 10000 == 1)
        {
            cout<<"Transaction : "<<transaction_id<<" done\n";
        }
    }
    cout<<"COMPRESSION DONE =====================================================================\n";
    //printing the compressed output
    print_pattern_map(inverse_pattern_map, outfilename, patterns_used);
    print_output(transaction_comp, outfilename);
    auto stop2 = chrono::high_resolution_clock::now();
    auto duration2 = chrono::duration_cast<chrono::microseconds>(stop2-start_comp);
	cout<<"Duration for compression is : "<<duration2.count()/(1000000.0)<<" seconds"<<endl;
    print_compression_ratio(transaction_comp, transactions_orig, inverse_pattern_map, patterns_used);
} 

int main(int argc, char** argv) {

    if(argc < 4)
        cout<<"Error in number of command line inputs"<<endl;
    string mode = argv[1];
    string infilename = argv[2];
    string outfilename = argv[3];
    auto start = chrono::high_resolution_clock::now();
    //Compression mode
    if(mode.compare("C") == 0) {
        std::vector<vector<int>>* read_transactions = read_file_into_transactions(infilename);
        cout<<"INPUT READ =====================================================================\n";
        float support_percent,sparseness;
        cout<<num_of_items/avg_trans_length<<endl;
        if(num_of_items/avg_trans_length > 500)
        {
            sparseness=(avg_trans_length*avg_trans_length*avg_trans_length*avg_trans_length)/num_of_items;
        }
        else{
            sparseness=(avg_trans_length*avg_trans_length)/num_of_items;
        }
        cout<<"SPARSENESS IS : "<<sparseness<<"\n";

        if(sparseness>50)
        support_percent=15;
        else if(sparseness>1)
        support_percent=1;
        else
        support_percent=0.1;

        cout<<"Support percent is "<<support_percent<<endl;
        
        float new_support;
        if(num_of_items>5e5)
        new_support=15;
        else{
        for(auto x : m)
        {
            v.push_back({x.second, x.first});
        }
        // 3000th largest element
        sort(v.begin(), v.end());
        reverse(v.begin(), v.end());

        int vv = (v.size() - 1)*0.8;
        new_support = (v[min(1000, vv)].first/num_of_transactions)*100;}

        cout<<"Support percent is NEW "<<new_support<<endl;


        FP_Tree_handler dm = FP_Tree_handler(new_support,infilename,outfilename);
        vector<vector<int>> *patterns = dm.run_fp_tree(max_checks);
        cout<<"PATTERNS MADE =====================================================================\n";

        auto stop_3 = chrono::high_resolution_clock::now();
        auto duration_3 = chrono::duration_cast<chrono::microseconds>(stop_3-start);
        cout<<"Duration for pattern_making is : "<<duration_3.count()/(1000000.0)<<" seconds"<<endl;
        compress_transactions(*read_transactions, *patterns, max_item_no, outfilename);
        cout<<"OUTPUT FILE DONE =====================================================================\n";
    } 
    //Decompression mode
    else 
    {
        decompress(infilename,outfilename);
        cout<<"DECOMPRESSED FILE DONE =====================================================================\n";
    }

    return 0;
}
