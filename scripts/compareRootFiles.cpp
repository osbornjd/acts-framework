// This ROOT script compares two ROOT files in an order-insensitive way. Its
// intended use is to compare the output of a single-threaded and multi-threaded
// programs in order to check that results are perfectly reproducible.
//
// As a current limitation, which may be lifted in the future, the script does
// all of its processing in RAM, which means that the input dataset must fit in
// RAM. So do not try to run this on terabytes of data. You don't need that much
// data to check that your multithreaded program runs well anyhow.
//
// Another limitation is that the comparison relies on perfect output
// reproducibility, which is a very costly guarantee to achieve in a
// multi-threaded environment. If you want to compare "slightly different"
// outputs, this script will not work as currently written. I cannot think of a
// way in which imperfect reproducibility could be checked in a manner which
// doesn't depend on the details of the data being compared.

#include <cstring>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "TBranch.h"
#include "TFile.h"
#include "TKey.h"
#include "TList.h"
#include "TObject.h"
#include "TTree.h"
#include "TTreeReader.h"

#include "compareRootFiles.hpp"


// Minimal mechanism for assertion checking and comparison
#define CHECK(pred, msg)            \
  if(!(pred)) {                     \
    std::cout << msg << std::endl;  \
    return 1;                       \
  }

#define CHECK_EQUAL(v1, v2, msg) \
  CHECK((v1) == (v2), msg)

#define CHECK_STR_EQUAL(s1, s2, msg) \
  CHECK(strcmp((s1), (s2)) == 0, msg)


// This script returns 0 if the files have identical contents except for event
// ordering, and a nonzero result if the contents differ or an error occured.
int compareRootFiles(std::string file1, std::string file2)
{
  std::cout << "Comparing ROOT files " << file1 << " and " << file2 << std::endl;


  std::cout << "* Opening the files..." << std::endl;
  HomogeneousPair<TFile> files{ file1.c_str(), file2.c_str() };


  std::cout << "* Extracting file keys..." << std::endl;
  HomogeneousPair<std::vector<TKey*>> fileKeys;
  {
    // This is how we would extract keys from one file
    const auto loadKeys = [](const TFile& file, std::vector<TKey*>& target)
    {
      const int keyCount = file.GetNkeys();
      target.reserve(keyCount);
      TIter keyIter{ file.GetListOfKeys() };
      for(int i = 0; i < keyCount; ++i) {
        target.emplace_back(dynamic_cast<TKey*>(keyIter()));
      }
    };
    
    // Do it for each of our files
    loadKeys(files.first, fileKeys.first);
    loadKeys(files.second, fileKeys.second);
  }


  std::cout << "* Selecting the latest cycle of each key..." << std::endl;
  std::vector<HomogeneousPair<TKey*>> keyPairs;
  {
    // For each file and for each key name, we want to know what is the latest
    // key cycle, and who is the associated key object
    using KeyMetadata = std::pair<short, TKey*>;
    using FileMetadata = std::map<std::string, KeyMetadata>;
    HomogeneousPair<FileMetadata> metadata;

    // This is how we compute this metadata for a single file
    const auto findLatestCycle= [](const std::vector<TKey*>& keys,
                                   FileMetadata& target)
    {
      // Iterate through the file's keys
      for(const auto key: keys)
      {
        // Extract information about the active key
        const std::string keyName{ key->GetName() };
        const short newCycle{ key->GetCycle() };

        // Do we already know of a key with the same name?
        auto latestCycleIter = target.find(keyName);
        if(latestCycleIter != target.end()) {
          // If so, keep the key with the most recent cycle number
          auto& latestCycleMetadata = latestCycleIter->second;
          if(newCycle > latestCycleMetadata.first) {
            latestCycleMetadata = {newCycle, key};
          }
        } else {
          // If not, this is obviously the most recent key we've seen so far
          target.emplace(keyName, KeyMetadata{newCycle, key});
        }
      }
    };

    // We'll compute this information for both of our files...
    std::cout << "  - Finding the latest cycle for each file..." << std::endl;
    findLatestCycle(fileKeys.first, metadata.first);
    findLatestCycle(fileKeys.second, metadata.second);

    // ...and then we'll group the latest keys by name, detect keys which only
    // exist in a single file along the way, and report that as an error
    std::cout << "  - Grouping per-file latest keys..." << std::endl;
    {
      // Make sure that both files have the same amount of keys once duplicate
      // versions are removed
      const auto f1KeyCount = metadata.first.size();
      const auto f2KeyCount = metadata.second.size();
      CHECK_EQUAL(f1KeyCount, f2KeyCount,
                  "    o Number of keys does not match");
      keyPairs.reserve(f1KeyCount);

      // Iterate through the keys, in the same order (as guaranteed by std::map)
      for(auto f1MetadataIter = metadata.first.cbegin(),
               f2MetadataIter = metadata.second.cbegin();
          f1MetadataIter != metadata.first.cend();
          ++f1MetadataIter, ++f2MetadataIter)
      {
        // Do the keys have the same name?
        const auto& f1KeyName = f1MetadataIter->first;
        const auto& f2KeyName = f2MetadataIter->first;
        CHECK_EQUAL(f1KeyName, f2KeyName,
                    "    o Key names do not match");

        // If so, extract the associated key pair
        keyPairs.emplace_back(f1MetadataIter->second.second,
                              f2MetadataIter->second.second);
      }
    }
  }


  std::cout << "* Comparing key metadata..." << std::endl;
  for(const auto& keyPair: keyPairs) {
    const auto& key1 = keyPair.first;
    const auto& key2 = keyPair.second;

    CHECK_STR_EQUAL(key1->GetClassName(), key2->GetClassName(),
                    "  - Class name does not match!");
    CHECK_STR_EQUAL(key1->GetTitle(), key2->GetTitle(),
                    "  - Title does not match!");
    CHECK_EQUAL(key1->GetVersion(), key2->GetVersion(),
                "  - Key version does not match!");
  }


  // NOTE: The current version of this script only supports TTree file contents.
  //       It may be extended later if the need for other data formats arise.
  std::cout << "* Extracting TTrees..." << std::endl;
  std::vector<HomogeneousPair<TTree*>> treePairs;
  for(const auto& keyPair: keyPairs) {
    TObject* obj1 = keyPair.first->ReadObj();
    TObject* obj2 = keyPair.second->ReadObj();

    CHECK_STR_EQUAL(obj1->ClassName(), obj2->ClassName(),
                    "  - Object type does not match!");
    CHECK_STR_EQUAL(obj1->ClassName(), "TTree",
                    "  - Non-TTree input is not supported!");

    treePairs.emplace_back(dynamic_cast<TTree*>(obj1),
                           dynamic_cast<TTree*>(obj2));
  }


  std::cout << "* Comparing the trees..." << std::endl;
  for(const auto& treePair: treePairs) {
    const auto& tree1 = treePair.first;
    const auto& tree2 = treePair.second;

    std::cout << "  - Comparing tree-wide metadata..." << std::endl;
    const std::size_t t1EntryCount = tree1->GetEntries();
    {
      const std::size_t t2EntryCount = tree2->GetEntries();
      CHECK_EQUAL(t1EntryCount, t2EntryCount,
                  "    o Number of entries does not match!");
    }

    std::cout << "  - Preparing for tree readout..." << std::endl;
    TTreeReader t1Reader(tree1);
    TTreeReader t2Reader(tree2);
    BranchComparisonHarness::TreeMetadata treeMetadata {
      t1Reader,
      t2Reader,
      t1EntryCount
    };

    std::cout << "  - Extracting branch metadata..." << std::endl;
    std::vector<HomogeneousPair<TBranch*>> branchPairs;
    {
      // Check number of branches and allocate branch storage
      const int t1BranchCount = tree1->GetNbranches();
      const int t2BranchCount = tree2->GetNbranches();
      CHECK_EQUAL(t1BranchCount, t2BranchCount,
                  "    o Number of branches does not match!");
      branchPairs.reserve(t1BranchCount);

      // Extract branches using TTree::GetListOfBranches()
      TIter t1BranchIter{ tree1->GetListOfBranches() };
      TIter t2BranchIter{ tree2->GetListOfBranches() };
      for(int i = 0; i < t1BranchCount; ++i) {
        branchPairs.emplace_back(dynamic_cast<TBranch*>(t1BranchIter()),
                                 dynamic_cast<TBranch*>(t2BranchIter()));
      }
    }

    std::cout << "  - Setting up branch-specific processing..." << std::endl;
    std::vector<BranchComparisonHarness> branchComparisonHarnesses;
    branchComparisonHarnesses.reserve(branchPairs.size());
    for(const auto& branchPair: branchPairs) {
      const auto& branch1 = branchPair.first;
      const auto& branch2 = branchPair.second;
      
      std::cout << "    o Checking branch metadata..." << std::endl;
      std::string b1ClassName, b1BranchName;
      EDataType b1DataType;
      {
        std::string b2ClassName, b2BranchName;
        EDataType b2DataType;
        TClass* unused;

        b1ClassName = branch1->GetClassName();
        b2ClassName = branch2->GetClassName();
        CHECK_EQUAL(b1ClassName, b2ClassName,
                    "      ~ Class name does not match!");
        branch1->GetExpectedType(unused, b1DataType);
        branch2->GetExpectedType(unused, b2DataType);
        CHECK_EQUAL(b1DataType, b2DataType,
                    "      ~ Raw data type does not match!");
        const int b1LeafCount = branch1->GetNleaves();
        const int b2LeafCount = branch2->GetNleaves();
        CHECK_EQUAL(b1LeafCount, b2LeafCount,
                    "      ~ Number of leaves does not match!");
        CHECK_EQUAL(b1LeafCount, 1,
                    "      ~ Branches with several leaves are not supported!");
        b1BranchName = branch1->GetName();
        b2BranchName = branch2->GetName();
        CHECK_EQUAL(b1BranchName, b2BranchName,
                    "      ~ Branch name does not match!");
      }

      std::cout << "    o Building branch comparison harness..." << std::endl;
      try {
        auto branchHarness = BranchComparisonHarness::create(treeMetadata,
                                                             b1BranchName,
                                                             b1DataType,
                                                             b1ClassName);
        branchComparisonHarnesses.emplace_back(std::move(branchHarness));
      } catch(...) {
        return 2;
      }
    }

    std::cout << "  - Reading event data..." << std::endl;
    for(std::size_t i = 0; i < t1EntryCount; ++i) {
      // Move to the next TTree entry (= next event)
      t1Reader.Next();
      t2Reader.Next();

      // Load the data associated with each branch
      for(auto& branchHarness: branchComparisonHarnesses) {
        branchHarness.loadCurrentEvent();
      }
    }

    std::cout << "  - Sorting the first tree..." << std::endl;
    {
      std::cout << "    o Defining event comparison operator..." << std::endl;
      IndexComparator t1CompareEvents = [&branchComparisonHarnesses]
                                        (std::size_t i, std::size_t j)
                                        -> Ordering
      {
        for(auto& branchHarness: branchComparisonHarnesses) {
          const auto order = branchHarness.sortHarness.first.first(i, j);
          if(order != Ordering::EQUAL) { return order; }
        }
        return Ordering::EQUAL;
      };

      std::cout << "    o Defining event swapping operator..." << std::endl;
      IndexSwapper t1SwapEvents = [&branchComparisonHarnesses]
                                  (std::size_t i, std::size_t j) {
        for(auto& branchHarness: branchComparisonHarnesses) {
          branchHarness.sortHarness.first.second(i, j);
        }
      };

      std::cout << "    o Running quicksort on the tree..." << std::endl;
      quickSort(0, t1EntryCount-1, t1CompareEvents, t1SwapEvents);
    }


    std::cout << "  - Sorting the second tree..." << std::endl;
    {
      std::cout << "    o Defining event comparison operator..." << std::endl;
      IndexComparator t2CompareEvents = [&branchComparisonHarnesses]
                                        (std::size_t i, std::size_t j)
                                        -> Ordering
      {
        for(auto& branchHarness: branchComparisonHarnesses) {
          const auto order = branchHarness.sortHarness.second.first(i, j);
          if(order != Ordering::EQUAL) { return order; }
        }
        return Ordering::EQUAL;
      };

      std::cout << "    o Defining event swapping operator..." << std::endl;
      IndexSwapper t2SwapEvents = [&branchComparisonHarnesses]
                                  (std::size_t i, std::size_t j) {
        for(auto& branchHarness: branchComparisonHarnesses) {
          branchHarness.sortHarness.second.second(i, j);
        }
      };

      std::cout << "    o Running quicksort on the tree..." << std::endl;
      quickSort(0, t1EntryCount-1, t2CompareEvents, t2SwapEvents);
    }


    std::cout << "  - Checking that both trees are now equal..." << std::endl;
    for(auto& branchHarness: branchComparisonHarnesses) {
      CHECK(branchHarness.eventDataEqual(),
            "    o Branch contents do not match!");
    }
  }


  std::cout << "* Input files are equal except for event order!" << std::endl;
  return 0;
}
