#ifndef GC_ADDRESSESHASHTABLE_H // header guard
#define GC_ADDRESSESHASHTABLE_H

#include "base.h"
#include "gc_memaddress.h"
#include <vector>
#include <cstdint>

namespace _3fd
{
	namespace memory
	{
		class MemBlock;

		/// <summary>
		/// This class uses hash table data structure (with open addressing and linear probing) to store information about 
		/// the <see cref="sptr" /> objects managed by the GC. It was not converted to a template because it was designed 
		/// very specifically (optimized) for its job. The implementation could be more "OOP/C++ like", but the concern here 
		/// is to save memory. If you find yourself wishing to change its model to make it more OOP compliant, remember it 
		/// was designed that way so as to save something around 8 or 16 bytes per element added to the table.
		/// </summary>
		class AddressesHashTable : notcopiable
		{
		public:

			/// <summary>
			/// This structure is the actual bucket element
			/// </summary>
			class Element
			{
			private:

				MemAddress m_sptrObjectAddr; // This is the unique key (the memory address of the sptr object)
				void *m_pointedAddr; // This is a value (where the sptr points to)

			public:

				/// <summary>
				/// Initializes a new instance of the <see cref="Element"/> class.
				/// </summary>
				Element()
					: m_sptrObjectAddr(nullptr), m_pointedAddr(nullptr) {}

				/// <summary>
				/// Constructor for the <see cref="Element" /> objects allocated on the stack.
				/// </summary>
				/// <param name="isRoot">
				/// When set to <c>true</c>, means the <see cref="sptr" /> object does not
				/// live on garbage collected memory (hence a "root" in the GC memory graph).
				/// </param>
				/// <param name="sptrObjectAddr">The <see cref="sptr" /> object address.</param>
				/// <param name="pointedAddr">The address pointed by the <see cref="sptr" /> object.</param>
				Element(bool isRoot, void *sptrObjectAddr, void *pointedAddr) :
					m_sptrObjectAddr(sptrObjectAddr),
					m_pointedAddr(pointedAddr)
				{
					if (isRoot)
						m_sptrObjectAddr.SetBit0(true);
				}

				void *GetSptrObjectAddr() const { return m_sptrObjectAddr.Get(); }

				bool IsRoot() const { return m_sptrObjectAddr.GetBit0(); }

				void *GetPointedAddr() const { return m_pointedAddr; }

				void SetAddrPointed(void *pointedAddr) { m_pointedAddr = pointedAddr; }
			};

		private:

			std::vector<Element> m_bucketArray;
			size_t m_elementsCount;
			uint32_t m_outHashSizeInBits;

			/// <summary>
			/// Calculates the load factor.
			/// </summary>
			/// <returns>The current load factor of the table.</returns>
			float CalculateLoadFactor() const
			{
				if (m_bucketArray.empty() == false)
					return ((float)m_elementsCount) / m_bucketArray.size();
				else
					return 0.0;
			}

			static size_t Hash(void *key);
			static size_t XorFold(size_t hash, uint32_t outHashSizeInBits);

			void ExpandTable();
			void ShrinkTable();

		public:

			AddressesHashTable();

			Element &Insert(bool isRoot, void *sptrObjectAddr, void *pointedAddr);

			// Lookup for an sptr object
			Element &Lookup(void *sptrObjectAddr);

			// Removes an entry from the hash table
			void Remove(void *sptrObjectAddr);
		};

	}// end of namespace memory
}// end of namespace _3fd

#endif // end of header guard
