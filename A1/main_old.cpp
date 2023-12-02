#include <cassert>
#include <cstdlib>
#include <iostream>
#include <iostream>
#include <set>
#include <vector>
#include <bits/stdc++.h>

#include "fptree.hpp"

using namespace std;

int max_item_no = 0;


// write the output in the output file
void print_output(vector<vector<int>> &v, string outfile)
{
    ofstream outputFile;
    outputFile.open(outfile);
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

// Printing the transactions
// for (const Transaction& transaction : *read_transactions) 
// {
//     for (const Item& item : transaction) 
//     {
//         std::cout << item << " ";
//     }
//     std::cout << std::endl;
// }

//reading the input file and making the transactions
std::vector<Transaction>* read_file_into_transactions(string file_name)
{
    std::ifstream myfile("./A1_datasets/" + file_name + ".dat");
    std::vector<Transaction>* transactions = new std::vector<Transaction>;
    std::string line;

    int tr_id = 0;

    if (myfile.is_open()) 
    {
        while (getline(myfile, line)) 
        {
            Transaction curr_transaction;
            std::istringstream iss(line);
            int num;
            while (iss >> num) 
            {
                max_item_no = max(num,max_item_no);
                curr_transaction.push_back(std::to_string(num));
            }
            transactions->push_back(curr_transaction);
        }
        std::cout<<transactions->size()<<std::endl;
        myfile.close();
    }
    return transactions;
}

// printing the frequent transactions along with their supports
void print_pattern(set<Pattern> p)
{
    cout<<p.size()<<"\n";
    for(auto i: p)
    {
        cout<<"Freq:"<<i.second<<"\n";
        cout<<"Elements : ";
        for(auto j:i.first)
        {
            cout<<j<<" ";
        }
        cout<<"\n";
    }
}

// Given the original transactions and the identified frequent itemsets, outputs the compressed transactions
void compress_transactions(const vector<Transaction> &transactions_orig, const set<Pattern> &patterns, int &max_item_no)
{
    vector<vector<int>> transaction_comp(transactions_orig.size(), vector<int>(0,0));
    map<int, int> item_frequencies;
    map<set<int>, int> pattern_map;
    int current_adder = 1;
    int transaction_id = 0;

    //storing the no of patterns in which each element occurs + assigning keys to patterns and inserting them into a map
    for(auto item_key_pair : patterns)
    {   
        set<int> current_set;
        for(auto curr_item : item_key_pair.first)
        {
            item_frequencies[stoi(std::string(curr_item))] ++;
            current_set.insert(stoi(std::string(curr_item)));
        }
        pattern_map[current_set] = max_item_no + current_adder;
        current_adder ++;
    }

    //compressing for each transaction individually
    for(auto transaction : transactions_orig)
    {   
        //sorting the transaction items based on the no of patterns in which they appear
        vector<pair<int, int>> curr_transaction_counts;
        for(auto curr_item : transaction)
        {
            curr_transaction_counts.push_back({item_frequencies[stoi(std::string(curr_item))], stoi(std::string(curr_item))});
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
                for(int next_item_id = item_id+1; next_item_id < curr_transaction_counts.size(); next_item_id++)
                {
                    //if this prospective element has not been seen yet
                    if(item_compressed[next_item_id] == false)
                    {
                        final_set.insert(curr_transaction_counts[next_item_id].second);
                        //if we have a frequent itemset
                        if(pattern_map[final_set] != 0)
                        {
                            current_key = pattern_map[final_set];
                            item_compressed[next_item_id] = true;
                        }
                        //if we dont have a frequent itemset
                        else
                        {
                            final_set.erase(final_set.find(curr_transaction_counts[next_item_id].second));
                        }
                    }
                }
                transaction_comp[transaction_id].push_back(current_key);
            }
        }
        transaction_id++;
    }
    //printing the compressed output
    print_output(transaction_comp, "out.dat");
} 

float compression_ratio(vector<vector<int>>&transaction_comp,vector<vector<int>> &v, map<set<int>, int> &pattern_map)
{   int size_of_transactions=0,size_of_comprtransactions=0,map_size=0;

    for(auto transaction:transactions_orig)
    size_of_transactions+=transaction.size();

    for(auto transaction:v)
    size_of_comprtransactions+=transaction.size();

    set<int>s;
    for(auto x:transaction_comp){
        for(auto y: x){
            s.insert(y);

        }
    }

    for(auto x: pattern_map){
        if(s.find(x.second)!=s.end())
        map_size+=x.first.size()+1;

    }

    float compression_ratio= float(size_of_comprtransactions+map_size)*100/size_of_transactions;
    return compression_ratio;

}

void test_1()
{
    const uint64_t minimum_support_threshold = 20;

    // Read the transactions
    const std::vector<Transaction>* read_transactions = read_file_into_transactions("D_temp");
    cout<<"INPUT READ =====================================================================\n";

    const FPTree fptree{ *read_transactions, minimum_support_threshold };
    const std::set<Pattern> patterns = fptree_growth( fptree );
    cout<<"PATTERNS MADE =====================================================================\n";

    compress_transactions(*read_transactions, patterns, max_item_no);
    cout<<"COMPRESSION DONE =====================================================================\n";

    print_pattern(patterns);
}

void test_2()
{
    const Item a{ "A" };
    const Item b{ "B" };
    const Item c{ "C" };
    const Item d{ "D" };
    const Item e{ "E" };

    const std::vector<Transaction> transactions{
        { a, b, d, e },
        { b, c, e },
        { a, b, d, e },
        { a, b, c, e },
        { a, b, c, d, e },
        { b, c, d },
    };

    const uint64_t minimum_support_threshold = 3;

    const FPTree fptree{ transactions, minimum_support_threshold };

    const std::set<Pattern> patterns = fptree_growth( fptree );
    

    assert( patterns.size() == 19 );
    assert( patterns.count( { { e, b }, 5 } ) );
    assert( patterns.count( { { e }, 5 } ) );
    assert( patterns.count( { { a, b, e }, 4 } ) );
    assert( patterns.count( { { a, b }, 4 } ) );
    assert( patterns.count( { { a, e }, 4 } ) );
    assert( patterns.count( { { a }, 4 } ) );
    assert( patterns.count( { { d, a, b }, 3 } ) );
    assert( patterns.count( { { d, a }, 3 } ) );
    assert( patterns.count( { { d, e, b, a }, 3 } ) );
    assert( patterns.count( { { d, e, b }, 3 } ) );
    assert( patterns.count( { { d, e, a }, 3 } ) );
    assert( patterns.count( { { d, e }, 3 } ) );
    assert( patterns.count( { { d, b }, 4 } ) );
    assert( patterns.count( { { d }, 4 } ) );
    assert( patterns.count( { { c, e, b }, 3 } ) );
    assert( patterns.count( { { c, e }, 3 } ) );
    assert( patterns.count( { { c, b }, 4 } ) );
    assert( patterns.count( { { c }, 4 } ) );
    assert( patterns.count( { { b }, 6 } ) );
}

void test_3()
{
    const Item a{ "A" };
    const Item b{ "B" };
    const Item c{ "C" };
    const Item d{ "D" };
    const Item e{ "E" };
    const Item f{ "F" };
    const Item g{ "G" };
    const Item h{ "H" };
    const Item i{ "I" };
    const Item j{ "J" };
    const Item k{ "K" };
    const Item l{ "L" };
    const Item m{ "M" };
    const Item n{ "N" };
    const Item o{ "O" };
    const Item p{ "P" };
    const Item s{ "S" };

    const std::vector<Transaction> transactions{
        { f, a, c, d, g, i, m, p },
        { a, b, c, f, l, m, o },
        { b, f, h, j, o },
        { b, c, k, s, p },
        { a, f, c, e, l, p, m, n }
    };

    const uint64_t minimum_support_threshold = 3;

    const FPTree fptree{ transactions, minimum_support_threshold };

    const std::set<Pattern> patterns = fptree_growth( fptree );

    assert( patterns.size() == 18 );
    assert( patterns.count( { { f }, 4 } ) );
    assert( patterns.count( { { c, f }, 3 } ) );
    assert( patterns.count( { { c }, 4 } ) );
    assert( patterns.count( { { b }, 3 } ) );
    assert( patterns.count( { { p, c }, 3 } ) );
    assert( patterns.count( { { p }, 3 } ) );
    assert( patterns.count( { { m, f, c }, 3 } ) );
    assert( patterns.count( { { m, f }, 3 } ) );
    assert( patterns.count( { { m, c }, 3 } ) );
    assert( patterns.count( { { m }, 3 } ) );
    assert( patterns.count( { { a, f, c, m }, 3 } ) );
    assert( patterns.count( { { a, f, c }, 3 } ) );
    assert( patterns.count( { { a, f, m }, 3 } ) );
    assert( patterns.count( { { a, f }, 3 } ) );
    assert( patterns.count( { { a, c, m }, 3 } ) );
    assert( patterns.count( { { a, c }, 3 } ) );
    assert( patterns.count( { { a, m }, 3 } ) );
    assert( patterns.count( { { a }, 3 } ) );
}

int main(int argc, const char *argv[])
{
    test_1();
    test_2();
    test_3();
    std::cout << "All tests passed!" << std::endl;

    return EXIT_SUCCESS;
}
